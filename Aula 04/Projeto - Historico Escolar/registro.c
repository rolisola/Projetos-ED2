#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "registro.h"

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

void carregar_insere(){
    FILE *arquivo_insere = abrir_criar_arquivo("insere.bin", "rb");
    if(arquivo_insere == NULL){
        printf("Erro ao abrir arquivo.\n");
    }
    REGISTRO reg;

    size_t tam_arquivo = obter_tamanho_arquivo(arquivo_insere);
    unsigned int qtd = tam_arquivo / sizeof(reg);
    printf("%u\n", qtd);
    //fread(&reg, sizeof(reg), 1, arquivo_insere);
    /*
    fread(&reg.id_Aluno, sizeof(reg.id_Aluno), 1, arquivo_insere);
    fread(&reg.sigla_Disciplina, sizeof(reg.sigla_Disciplina), 1, arquivo_insere);
    fread(&reg.nome_Aluno, sizeof(reg.nome_Aluno), 1, arquivo_insere);
    fread(&reg.nome_Disciplina, sizeof(reg.nome_Disciplina), 1, arquivo_insere);
    fread(&reg.media, sizeof(reg.media), 1, arquivo_insere);
    fread(&reg.frequencia, sizeof(reg.frequencia), 1, arquivo_insere);
    */
    /*
    printf("id_aluno: (%s)\n",reg.id_Aluno);
    printf("sigla: (%s)\n",reg.sigla_Disciplina);
    printf("nome Aluno: (%s)\n",reg.nome_Aluno);
    printf("Nome disciplina: (%s)\n",reg.nome_Disciplina);
    printf("media: (%.2f)\n",reg.media);
    printf("freq: (%.2f)\n",reg.frequencia);
    */


}

size_t contar_registros(const char *nome_arquivo){
    FILE *arquivo = abrir_arquivo(nome_arquivo, "rb");

    size_t tamanho_registro = sizeof(REGISTRO);
    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo);

    if(arquivo == NULL){
        perror("Erri ao abrir arquivo.");
    }
    if(tamanho_registro == 0){
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
    /*if(arquivo == NULL){
        arquivo = fopen(nome_Arquivo, "wb+");
        int offset_cabecalho = -1; // offset_cabecalho indica a posicao do proximo espaco de registro disponivel, -1 indica final da lista encadeada (nao ha proximo espaco)
        fwrite(&offset_cabecalho, sizeof(int), 1, arquivo);
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
    int tamanhoRegistro = sizeof(Registro) + 2 * sizeof(float) + 4 * sizeof(char) + 4 * sizeof(int);
    int tamanhoTotal = tamanhoRegistro + sizeof(int) + 1; // Inclui '#' e tamanho

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
    fseek(arquivo, sizeof(int), SEEK_SET);
    int tamanhoTotal;
    Registro reg;

    while (fread(&tamanhoTotal, sizeof(int), 1, arquivo) > 0) {
        fread(&reg, sizeof(Registro), 1, arquivo);
        if (strncmp(reg.idAluno, idAluno, FIXO_ID) == 0 && strncmp(reg.siglaDisciplina, siglaDisciplina, FIXO_SIGLA) == 0) {
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
        fseek(arquivo, tamanhoTotal - sizeof(Registro) - sizeof(int), SEEK_CUR);
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
        fseek(arquivo, tamanhoTotal - sizeof(Registro) - sizeof(int), SEEK_CUR);
    }

    fclose(arquivo);
    fclose(arquivoTemp);

    remove(nomeArquivo);
    rename("temp.bin", nomeArquivo);

    carregarArquivo(nomeArquivo);
}*/
