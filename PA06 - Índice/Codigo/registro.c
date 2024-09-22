#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "arquivo.h"

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

void inserir_registro(const char *nome_arquivo_historico, REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    int posicao = obter_auxiliar(0);
    printf("Posição atual: %d\n", posicao);

    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }

    char string_completa[256];
    char string_texto[256];
    // imprime_vetor_insere(vetor_insere);
    snprintf(string_texto, sizeof(string_texto), "%s#%s#%s#%s",
             vetor_insere[posicao].id_Aluno,
             vetor_insere[posicao].sigla_Disciplina,
             vetor_insere[posicao].nome_Aluno,
             vetor_insere[posicao].nome_Disciplina);
    // int tamanho_string = strlen(string_texto);
    // printf("%d%s\n", (int)strlen(string_texto), string_texto);

    snprintf(string_completa, sizeof(string_completa), "%s#%.1f#%.1f",
             string_texto,
             vetor_insere[posicao].media,
             vetor_insere[posicao].frequencia);

    // printf("%d%s\n", (int)strlen(string_completa), string_completa);
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

int obter_auxiliar(int posicao) {
    FILE *arquivo_auxiliar = abrir_criar_arquivo("auxiliar.bin", "rb");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    int valor;

    fseek(arquivo_auxiliar, posicao, 0);
    fread(&valor, sizeof(int), 1, arquivo_auxiliar);

    fclose(arquivo_auxiliar);

    return valor;
}

void atualizar_auxiliar(int posicao, int valor) {
    FILE *arquivo_auxiliar = abrir_arquivo("auxiliar.bin", "wb+");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }
    // int val;
    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    // rewind(arquivo_auxiliar);
    // fread(&val,sizeof(int),1,arquivo_auxiliar);
    // printf("\n%d\n", valor);
    fclose(arquivo_auxiliar);
}

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

/*

void salvarEstado(Estado estado) {
    FILE *estadoFile = fopen("estado.bin", "wb+");
    fwrite(&estado, sizeof(Estado), 1, estadoFile);
    fclose(estadoFile);
}

Estado carregarEstado() {
    Estado estado = {0, 0};  // Inicializa com 0 caso o arquivo nao exista

    FILE *estadoFile = fopen("estado.bin", "rb");
    if (estadoFile != NULL) {
        fread(&estado, sizeof(Estado), 1, estadoFile);
        fclose(estadoFile);
    }

    return estado;
}

void inserirRegistro(FILE *arquivo, Registro reg, Estado *estado) {
    fseek(arquivo, 0, SEEK_SET);

    int offset_cabecalho;
    fread(&offset_cabecalho, sizeof(int), 1, arquivo);

    fseek(arquivo, 0, SEEK_END);
    int tamanhoRegistro = sizeof(Registro) + 2 * sizeof(float) + 4 *
sizeof(char) + 4 * sizeof(int); int tamanhoTotal = tamanhoRegistro +
sizeof(int) + 1; // Inclui '#' e tamanho

    if (offset_cabecalho == -1) {
        // Adicionar no final
        fwrite(&tamanhoTotal, sizeof(int), 1, arquivo);
        fwrite(&reg, sizeof(Registro), 1, arquivo);
    } else {
        // Implementacao de first-fit (simplificada)
        fseek(arquivo, offset_cabecalho, SEEK_SET);
        EspacoDisponivel espaco;
        fread(&espaco, sizeof(EspacoDisponivel), 1, arquivo);

        if (espaco.tamanho >= tamanhoTotal) {
            fseek(arquivo, offset_cabecalho, SEEK_SET);
            fwrite(&tamanhoTotal, sizeof(int), 1, arquivo);
            fwrite(&reg, sizeof(Registro), 1, arquivo);
        } else {
            fseek(arquivo, 0, SEEK_END);
            fwrite(&tamanhoTotal, sizeof(int), 1, arquivo);
            fwrite(&reg, sizeof(Registro), 1, arquivo);
        }
    }

    // Atualiza a posicao de insercao no estado
    estado->posInsercao++;
    salvarEstado(*estado);
}

void removerRegistro(FILE *arquivo, char *idAluno, char *siglaDisciplina, Estado *estado) {
    fseek(arquivo, sizeof(int), SEEK_SET); int tamanhoTotal;
    Registro reg;

    while (fread(&tamanhoTotal, sizeof(int), 1, arquivo) > 0) {
        fread(&reg, sizeof(Registro), 1, arquivo);
        if (strncmp(reg.idAluno, idAluno, FIXO_ID) == 0 &&
            strncmp(reg.siglaDisciplina, siglaDisciplina, FIXO_SIGLA) == 0) {
            // Remover e marcar espaco disponivel
            fseek(arquivo, -sizeof(Registro) - sizeof(int), SEEK_CUR);
            int offsetAtual = ftell(arquivo);
            fwrite(&tamanhoTotal, sizeof(int), 1, arquivo);
            int proxOffset = -1; // ultimo da lista
            fwrite(&proxOffset, sizeof(int), 1, arquivo);

            // Atualizar cabecalho
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&offsetAtual, sizeof(int), 1, arquivo);
            break;
        }
        fseek(arquivo, tamanhoTotal - sizeof(Registro) - sizeof(int),
SEEK_CUR);
    }

    // Atualiza a posicao de remocao no estado
    estado->posRemocao++;
    salvarEstado(*estado);
}

void compactarArquivo(const char* nome_Arquivo) {
    FILE *arquivo = carregarArquivo(nome_Arquivo);
    FILE *arquivoTemp = fopen("temp.bin", "wb+");
    int offset_cabecalho;
    fseek(arquivo, 0, SEEK_SET);
    fread(&offset_cabecalho, sizeof(int), 1, arquivo);

    fseek(arquivo, sizeof(int), SEEK_SET);
    int tamanhoTotal;
    Registro reg;

    while (fread(&tamanhoTotal, sizeof(int), 1, arquivo) > 0) {
        fread(&reg, sizeof(Registro), 1, arquivo);
        if (reg.idAluno[0] != '*') {
            fwrite(&tamanhoTotal, sizeof(int), 1, arquivoTemp);
            fwrite(&reg, sizeof(Registro), 1, arquivoTemp);
        }
        fseek(arquivo, tamanhoTotal - sizeof(Registro) - sizeof(int),
SEEK_CUR);
    }

    fclose(arquivo);
    fclose(arquivoTemp);

    remove(nomeArquivo);
    rename("temp.bin", nomeArquivo);

    carregarArquivo(nomeArquivo);
}*/
/*
#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3
*/
/*
typedef struct {
    char idAluno[FIXO_ID + 1];
    char siglaDisciplina[FIXO_SIGLA + 1];
    char nomeAluno[MAX_NOME + 1];
    char nomeDisciplina[MAX_NOME + 1];
    float media;
    float frequencia;
} Registro;

typedef struct {
    int tamanho;
    int prox;
} EspacoDisponivel;

typedef struct {
    int posInsercao;
    int posRemocao;
} Estado;
*/

// FUNÇÕES ALTERNATIVAS
/*REGISTRO* carregar_insere(){
    FILE *arquivo_insere = abrir_arquivo("insere.bin", "rb");
    if(arquivo_insere == NULL){
        printf("Erro ao abrir arquivo. SAINDO!!!!\n");
        exit(EXIT_FAILURE);
    }

    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo_insere);
    unsigned int qtd = tamanho_arquivo / sizeof(REGISTRO);

    REGISTRO *vetor_insere = (REGISTRO*) malloc(qtd * sizeof(REGISTRO));
    for(int i = 0; i < qtd; i++){
        fread(&vetor_insere[i], sizeof(REGISTRO), 1, arquivo_insere);
    }

    fclose(arquivo_insere);

    return vetor_insere;
}

void inserir_registro(const char *nome_arquivo_historico, REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    int posicao = obter_auxiliar(0);
    printf("Posição atual: %d\n", posicao);

    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }

    // Calculando o tamanho necessário para a string
    int tamanho_strings = strlen(vetor_insere[posicao].id_Aluno) + strlen(vetor_insere[posicao].sigla_Disciplina) + strlen(vetor_insere[posicao].nome_Aluno) + strlen(vetor_insere[posicao].nome_Disciplina);

    int tamanho_buffer = tamanho_strings + 4 + 4 + 5; // Contando os separadores e os floats
    int tamanho_total = tamanho_buffer + 1;           // Para o caractere nulo

    char *string_intermediaria = (char *)malloc(tamanho_total * sizeof(char));
    if (string_intermediaria == NULL) {
        perror("Erro ao alocar memória.");
        exit(EXIT_FAILURE);
    }

    snprintf(string_intermediaria,
             tamanho_total,
             "%s#%s#%s#%s#%.1f#%.1f",
             vetor_insere[posicao].id_Aluno,
             vetor_insere[posicao].sigla_Disciplina,
             vetor_insere[posicao].nome_Aluno,
             vetor_insere[posicao].nome_Disciplina,
             vetor_insere[posicao].media,
             vetor_insere[posicao].frequencia);

    int tamanho_string = strlen(string_intermediaria);
    int digitos_tamanho = snprintf(NULL, 0, "%d", tamanho_string);

    int tamanho_string_final = digitos_tamanho + tamanho_string + 1;
    char *string_final = (char *)malloc(tamanho_string_final * sizeof(char));
    if (string_final == NULL) {
        perror("Erro ao alocar memória.");
        free(string_intermediaria);
        exit(EXIT_FAILURE);
    }

    char str_aux[20];
    sprintf(str_aux, "%d", tamanho_string);

    snprintf(string_final,
             tamanho_string_final,
             "%d%s",
             tamanho_string + (int)strlen(str_aux),
             string_intermediaria);

    printf("String final: (%s)\nTamanho: (%d)\n",
           string_final,
           (int)strlen(string_final));

    posicao++;
    atualizar_auxiliar(0, posicao); // Atualiza a posição

    FILE *arquivo_historico = abrir_criar_arquivo(nome_arquivo_historico, "rb+");
    if (arquivo_historico == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        free(string_intermediaria);
        free(string_final);
        exit(EXIT_FAILURE);
    }
    fseek(arquivo_historico, 0, SEEK_END);
    printf("aaaaaaaaaaaaa %s\n", string_final);
    size_t escrito = fwrite(string_final, sizeof(char), strlen(string_final), arquivo_historico);
    if(escrito != 1){
        perror("Erro ao escrever no arquivo.");
        fclose(arquivo_historico);
        exit(EXIT_FAILURE);
    }
    fclose(arquivo_historico);

    free(string_intermediaria);
    free(string_final);
}

*/
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
