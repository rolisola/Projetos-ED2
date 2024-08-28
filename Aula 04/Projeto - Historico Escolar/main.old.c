#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3

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

FILE* carregarArquivo(const char* nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb+");
    if (arquivo == NULL) {
        arquivo = fopen(nomeArquivo, "wb+");
        int offset_cabecalho = -1;
        fwrite(&offset_cabecalho, sizeof(int), 1, arquivo);
    }
    return arquivo;
}

void fecharArquivo(FILE *arquivo) {
    fclose(arquivo);
}

void inserirRegistro(FILE *arquivo, Registro reg) {
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
        // Implementação de first-fit (simplificada)
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
}

void removerRegistro(FILE *arquivo, char *idAluno, char *siglaDisciplina) {
    fseek(arquivo, sizeof(int), SEEK_SET);
    int tamanhoTotal;
    Registro reg;

    while (fread(&tamanhoTotal, sizeof(int), 1, arquivo) > 0) {
        fread(&reg, sizeof(Registro), 1, arquivo);
        if (strncmp(reg.idAluno, idAluno, FIXO_ID) == 0 && strncmp(reg.siglaDisciplina, siglaDisciplina, FIXO_SIGLA) == 0) {
            // Remover e marcar espaço disponível
            fseek(arquivo, -sizeof(Registro) - sizeof(int), SEEK_CUR);
            int offsetAtual = ftell(arquivo);
            fwrite(&tamanhoTotal, sizeof(int), 1, arquivo);
            int proxOffset = -1; // Último da lista
            fwrite(&proxOffset, sizeof(int), 1, arquivo);

            // Atualizar cabeçalho
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&offsetAtual, sizeof(int), 1, arquivo);
            break;
        }
        fseek(arquivo, tamanhoTotal - sizeof(Registro) - sizeof(int), SEEK_CUR);
    }
}

void compactarArquivo(const char* nomeArquivo) {
    FILE *arquivo = carregarArquivo(nomeArquivo);
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
}

int main() {
    const char *nomeArquivo = "historico.bin";
    FILE *arquivo = carregarArquivo(nomeArquivo);

    // Exemplo de inserção
    Registro reg1 = {"001", "ED2", "Paulo da Silva", "Estrutura de Dados 2", 7.3, 75.4};
    inserirRegistro(arquivo, reg1);

    // Exemplo de remoção
    removerRegistro(arquivo, "001", "ED2");

    // Exemplo de compactação
    compactarArquivo(nomeArquivo);

    fecharArquivo(arquivo);
    return 0;
}
