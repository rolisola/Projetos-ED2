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
void inserir_registro(const char *nome_arquivo_dados, REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    int posicao = obter_auxiliar(0);
    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }

    printf("Inserindo registro: %d\n", posicao + 1);

    char string_buffer[256];
    // Preenchendo o buffer
    snprintf(string_buffer, sizeof(string_buffer), "%s#%s#%s#%s#%.1f#%.1f",
             vetor_insere[posicao].id_Aluno,
             vetor_insere[posicao].sigla_Disciplina,
             vetor_insere[posicao].nome_Aluno,
             vetor_insere[posicao].nome_Disciplina,
             vetor_insere[posicao].media,
             vetor_insere[posicao].frequencia);
    printf("%d%s\n", (int)strlen(string_buffer), string_buffer);

    int tamanho_string = strlen(string_buffer);

    posicao++;
    atualizar_auxiliar(0, posicao);

    FILE *arquivo_dados = abrir_criar_arquivo(nome_arquivo_dados, "ab+");

    fwrite(&tamanho_string, sizeof(int), 1, arquivo_dados);
    fwrite(string_buffer, strlen(string_buffer), 1, arquivo_dados);
    fclose(arquivo_dados);
}

/*
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
}*/

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

/*
void inserir_registro(REGISTRO *vetor_insere){
    int posicao = obter_auxiliar(0);
    printf("posicao: %d\n", posicao);
    // Calculando o tamanho necessário para a string
    int tamanho_strings = strlen(vetor_insere[posicao].id_Aluno) +
                          strlen(vetor_insere[posicao].sigla_Disciplina) +
                          strlen(vetor_insere[posicao].nome_Aluno) +
                          strlen(vetor_insere[posicao].nome_Disciplina);
    // Contando os separadores (#) e os dois floats com precisão %.1f (aproximadamente 4 caracteres cada)
    int tamanho_buffer = tamanho_strings + 4 + 4 + 5; // 5 separadores #
    // Criando espaço extra para o caractere nulo \0
    int tamanho_total = tamanho_buffer + 1; // Para o \0
    // Alocando a memória para o string_buffer
    char *string_buffer = (char *)malloc(tamanho_total * sizeof(char));
    if(string_buffer == NULL){
        printf("Erro ao alocar memória.\n");
        return;
    }
    // Preenchendo o buffer
    snprintf(string_buffer, tamanho_total, "%s#%s#%s#%s#%.1f#%.1f",
             vetor_insere[posicao].id_Aluno,
             vetor_insere[posicao].sigla_Disciplina,
             vetor_insere[posicao].nome_Aluno,
             vetor_insere[posicao].nome_Disciplina,
             vetor_insere[posicao].media,
             vetor_insere[posicao].frequencia);
    // Calculando o comprimento da string gerada e quantos dígitos são necessários para armazenar esse tamanho
    int tamanho_string = strlen(string_buffer);
    int digitos_tamanho = snprintf(NULL, 0, "%d", tamanho_string); // Quantidade de dígitos do tamanho
    // Criando espaço para a nova string com o tamanho incluído no início
    int tamanho_string_final = digitos_tamanho + tamanho_string + 1; // +1 para o \0
    char *new_string_buffer = (char *)malloc(tamanho_string_final * sizeof(char));
    if(new_string_buffer == NULL){
        printf("Erro ao alocar memória.\n");
        free(string_buffer); // Liberando string_buffer em caso de erro
        return;
    }
    snprintf(new_string_buffer, tamanho_string_final, "%d%s", tamanho_string, string_buffer);
    // Exibindo as strings resultantes
    printf("String final: (%s)\nTamanho: (%d)\n", new_string_buffer, (int)strlen(new_string_buffer));
    posicao++;
    atualizar_auxiliar(0, posicao);
    // Liberando a memória alocada
    free(string_buffer);
    free(new_string_buffer);
}
*/

/*void pesquisar_por_chave_primaria(const char *nome_arquivo_historico, const char *nome_arquivo_indice, const char *nome_arquivo_busca_primaria) {
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
}*/

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
