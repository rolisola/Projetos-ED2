#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "arquivo.h"

// Carrega e retorna um vetor de registros do insere.bin
REGISTRO *carregar_insere() {
    FILE *arquivo_insere = abrir_arquivo("insere.bin", "rb");
    if (arquivo_insere == NULL) {
        perror("Erro ao abrir arquivo.");
        exit(EXIT_FAILURE);
    }

    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo_insere);
    size_t quantidade_registros = tamanho_arquivo / sizeof(REGISTRO);

    REGISTRO *vetor_insere = (REGISTRO *)malloc(quantidade_registros * sizeof(REGISTRO));
    if (vetor_insere == NULL) {
        perror("Erro ao alocar memória.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < quantidade_registros; i++) {
        fread(&vetor_insere[i], sizeof(REGISTRO), 1, arquivo_insere);
    }

    fclose(arquivo_insere);

    return vetor_insere;
}

// Insere o registro do vetor no historico.bin
void inserir_registro(const char *nome_arquivo_historico, REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    int posicao = obter_auxiliar(0);

    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }

    char string_completa[256];
    char string_texto[256];

    snprintf(string_texto, sizeof(string_texto), "%s#%s#%s#%s",
             vetor_insere[posicao].id_Aluno,
             vetor_insere[posicao].sigla_Disciplina,
             vetor_insere[posicao].nome_Aluno,
             vetor_insere[posicao].nome_Disciplina);

    snprintf(string_completa, sizeof(string_completa), "%s#%.1f#%.1f",
             string_texto,
             vetor_insere[posicao].media,
             vetor_insere[posicao].frequencia);

    FILE *arquivo_historico = abrir_criar_arquivo(nome_arquivo_historico, "ab+");
    if (arquivo_historico == NULL) {
        perror("Erro ao abrir arquivo.");
        return;
    }

    int tamanho_string = strlen(string_completa);
    fseek(arquivo_historico, 0, SEEK_END);
    size_t escrito = fwrite(&tamanho_string, sizeof(int), 1, arquivo_historico);
    if (escrito != 1) {
        perror("Erro ao escrever no arquivo.");
        fclose(arquivo_historico);
        exit(EXIT_FAILURE);
    }
    escrito = fwrite(string_texto, strlen(string_texto) - 1, 1, arquivo_historico);
    if (escrito != 1) {
        perror("Erro ao escrever no arquivo.");
        fclose(arquivo_historico);
        exit(EXIT_FAILURE);
    }
    escrito = fwrite(&vetor_insere[posicao].media, sizeof(vetor_insere[posicao].media), 1, arquivo_historico);
    if (escrito != 1) {
        perror("Erro ao escrever no arquivo.");
        fclose(arquivo_historico);
        exit(EXIT_FAILURE);
    }
    escrito = fwrite(&vetor_insere[posicao].frequencia, sizeof(vetor_insere[posicao].frequencia), 1, arquivo_historico);
    if (escrito != 1) {
        perror("Erro ao escrever no arquivo.");
        fclose(arquivo_historico);
        exit(EXIT_FAILURE);
    }

    posicao++;
    atualizar_auxiliar(0, posicao);

    fclose(arquivo_historico);
}

// Obtem o valor de quantos registros já foram gravados
// Possiveis valores de entrada: 0 (insere.bin), 1 (busca_p.bin), 2 (busca_s.bin)
int obter_auxiliar(int posicao) {
    FILE *arquivo_auxiliar = abrir_criar_arquivo("auxiliar.bin", "rb");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    int valor;

    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fread(&valor, sizeof(int), 1, arquivo_auxiliar);
    fclose(arquivo_auxiliar);

    return valor;
}

// Atualiza valor de quantos registros foram usados do insere.bin, busca_p.bin e busca_s.bin
void atualizar_auxiliar(int posicao, int valor) {
    FILE *arquivo_auxiliar = abrir_arquivo("auxiliar.bin", "wb+");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    fclose(arquivo_auxiliar);
}

// Conta a quantidade de registros existentes no arquivo
size_t contar_registros(const char *nome_arquivo) {
    FILE *arquivo = abrir_arquivo(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo.");
        exit(EXIT_FAILURE);
    }

    size_t tamanho_registro = sizeof(REGISTRO);
    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo);

    if (tamanho_registro == 0) {
        fclose(arquivo);
        return 0; // Evita divisão por zero
    }

    size_t num_registros = tamanho_arquivo / tamanho_registro;

    fclose(arquivo);

    return num_registros;
}

// Função de comparação para ordenar e buscar usando keysorting
int comparador_chave_primaria(const void *a, const void *b) {
    IndicePrimario *ia = (IndicePrimario *)a;
    IndicePrimario *ib = (IndicePrimario *)b;

    int resultado_id = strcmp(ia->id_Aluno, ib->id_Aluno);
    if (resultado_id != 0)
        return resultado_id;

    return strcmp(ia->sigla_Disciplina, ib->sigla_Disciplina);
}

void pesquisar_por_chave_primaria(FILE *dados, FILE *indice, FILE *busca_p) {
    ChavePrimaria chave_busca;
    IndicePrimario *entradas_indice;
    int quantidade_registros, encontrado = 0;

    // Abrir o arquivo de índice
    indice = fopen("indice_primario.dat", "rb");
    if (indice == NULL) {
        printf("Erro ao abrir o arquivo de índice primário.\n");
        return;
    }

    // Carregar todas as entradas de índice para a memória
    fseek(indice, 0, SEEK_END);
    long tamanho_arquivo = ftell(indice);
    fseek(indice, 0, SEEK_SET);

    quantidade_registros = tamanho_arquivo / sizeof(IndicePrimario);
    entradas_indice = (IndicePrimario *)malloc(quantidade_registros * sizeof(IndicePrimario));
    if (entradas_indice == NULL) {
        perror("Erro ao alocar memória para o índice.\n");
        fclose(indice);
        return;
    }

    fread(entradas_indice, sizeof(IndicePrimario), quantidade_registros, indice);
    fclose(indice);

    // Ordenar as entradas utilizando keysorting
    qsort(entradas_indice, quantidade_registros, sizeof(IndicePrimario), comparador_chave_primaria);

    // Abrir o arquivo busca_p.bin para ler as chaves de busca
    busca_p = fopen("busca_p.bin", "rb");
    if (busca_p == NULL) {
        printf("Erro ao abrir o arquivo de busca_p.bin.\n");
        free(entradas_indice);
        return;
    }

    // Ler as chaves primárias (ID do aluno + Sigla da disciplina) do arquivo binário busca_p.bin
    while (fread(&chave_busca, sizeof(ChavePrimaria), 1, busca_p) > 0) {
        // Realizar a busca binária para cada chave do arquivo
        IndicePrimario chave_para_busca;
        strcpy(chave_para_busca.id_Aluno, chave_busca.id_Aluno);
        strcpy(chave_para_busca.sigla_Disciplina, chave_busca.sigla_Disciplina);

        IndicePrimario *resultado = (IndicePrimario *)bsearch(&chave_para_busca, entradas_indice,
                                                              quantidade_registros, sizeof(IndicePrimario),
                                                              comparador_chave_primaria);

        if (resultado == NULL) {
            printf("Registro com ID '%s' e Sigla '%s' não encontrado.\n",
                   chave_busca.id_Aluno, chave_busca.sigla_Disciplina);
        } else {
            // Se encontrou, posiciona o ponteiro do arquivo de dados no offset encontrado
            fseek(dados, resultado->offset, SEEK_SET);

            // Ler o registro do arquivo de dados
            REGISTRO reg;
            fread(&reg, sizeof(REGISTRO), 1, dados);

            // Exibir os dados do registro
            printf("\nRegistro encontrado:\n");
            printf("ID: %s\n", reg.id_Aluno);
            printf("Sigla da Disciplina: %s\n", reg.sigla_Disciplina);
            printf("Nome do Aluno: %s\n", reg.nome_Aluno);
            printf("Nome da Disciplina: %s\n", reg.nome_Disciplina);
            printf("Média: %.2f\n", reg.media);
            printf("Frequência: %.2f\n", reg.frequencia);
        }
    }

    fclose(busca_p);
    free(entradas_indice);
}

// Carrega o arquivo de histórico
/*FILE *carregar_Historico(const char* nome_Arquivo){

    FILE *arquivo = abrir_criar_arquivo(nome_Arquivo, "wb+");
    if(fread()){

    }
    if(arquivo == NULL){
        arquivo = fopen(nome_Arquivo, "wb+");
        int offset_cabecalho = -1; // offset_cabecalho indica a posicao do proximo espaco de registro disponivel, -1 indica final da lista encadeada (nao ha proximo espaco) fwrite(&offset_cabecalho, sizeof(int), 1, arquivo);
    }

    return arquivo;
}*/

// FUNÇÕES AUXILIARES

void imprime_vetor_insere(REGISTRO *vetor_insere) {
    for (int i = 0; i < sizeof(vetor_insere); i++) {
        printf("id_aluno: (%s)\n", vetor_insere[i].id_Aluno);
        printf("sigla: (%s)\n", vetor_insere[i].sigla_Disciplina);
        printf("nome Aluno: (%s)\n", vetor_insere[i].nome_Aluno);
        printf("Nome disciplina: (%s)\n", vetor_insere[i].nome_Disciplina);
        printf("media: (%.1f)\n", vetor_insere[i].media);
        printf("freq: (%.1f)\n", vetor_insere[i].frequencia);
        break;
    }
}
