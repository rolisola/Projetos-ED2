#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<assert.h>

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
    char nome_Aluno[MAX_NOME];
    char nome_Disciplina[MAX_NOME];
    float media;
    float frequencia;
} Registro;

#define MAX_CHAVE 7 // ID + Sigla (3 + 3) + '\0'
#define TAMANHO_HASH 13
#define BUCKET_SIZE 2
#define MARKER_REMOVIDO "#####"

typedef struct {
    char chave[MAX_CHAVE];
    int rrn;
} HashEntry;

typedef struct {
    HashEntry entradas[BUCKET_SIZE];
} Bucket;

int hash(char *chave) {
    int soma = 0;
    for (int i = 0; i < MAX_CHAVE - 1; i++) {
        soma += chave[i];
    }
    return soma % TAMANHO_HASH;
}

// FUNÇÕES DE HASH
int hash_function(char *chave);
void inserir_hash(char *chave, int rrn);
void buscar_hash(char *chave);
void remover_hash(char *chave);

// FUNÇÕES  DE REGISTRO
void inserir_registro(char *chave, char *nome_aluno, char *nome_disciplina, float media, float frequencia);
void buscar_registro(char *chave);
void remover_registro(char *chave);

int main() {
    char opcao;
    char chave[MAX_CHAVE], nome_aluno[MAX_NOME], nome_disciplina[MAX_NOME];
    float media, frequencia;

    while (1) {
        printf("\nMenu:\n");
        printf("a - Inserir registro\n");
        printf("b - Buscar registro\n");
        printf("c - Remover registro\n");
        printf("q - Sair\n");
        printf("Escolha uma opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'a') {
            printf("Digite a chave (ID+Sigla): ");
            scanf("%s", chave);
            printf("Digite o nome do aluno: ");
            scanf("%s", nome_aluno);
            printf("Digite o nome da disciplina: ");
            scanf("%s", nome_disciplina);
            printf("Digite a média: ");
            scanf("%f", &media);
            printf("Digite a frequência: ");
            scanf("%f", &frequencia);
            inserir_hash(chave, hash_function(chave));
            inserir_registro(chave, nome_aluno, nome_disciplina, media, frequencia);

        } else if (opcao == 'b') {
            printf("Digite a chave (ID+Sigla) para busca: ");
            scanf("%s", chave);
            buscar_hash(chave);
            buscar_registro(chave);

        } else if (opcao == 'c') {
            printf("Digite a chave (ID+Sigla) para remoção: ");
            scanf("%s", chave);
            remover_hash(chave);
            remover_registro(chave);

        } else if (opcao == 'q') {
            printf("Saindo...\n");
            break;

        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}

int hash_function(char *chave) {
    int soma = 0;
    for (int i = 0; i < strlen(chave); i++) {
        soma += chave[i];
    }
    return soma % TAMANHO_HASH;
}

void inserir_hash(char *chave, int rrn) {
    FILE *file = fopen("index.dat", "a");
    if (file) {
        fprintf(file, "%s %d\n", chave, rrn);
        fclose(file);
    }
}

void buscar_hash(char *chave) {
    FILE *file = fopen("index.dat", "r");
    char buffer[MAX_CHAVE];
    int rrn;

    if (file) {
        while (fscanf(file, "%s %d", buffer, &rrn) != EOF) {
            if (strcmp(buffer, chave) == 0) {
                printf("Chave %s encontrada, RRN %d\n", chave, rrn);
                fclose(file);
                return;
            }
        }
        printf("Chave %s não encontrada\n", chave);
        fclose(file);
    }
}

void remover_hash(char *chave) {
    FILE *file = fopen("index.dat", "r+");
    char buffer[MAX_CHAVE];
    int rrn;

    if (file) {
        while (fscanf(file, "%s %d", buffer, &rrn) != EOF) {
            if (strcmp(buffer, chave) == 0) {
                fseek(file, -strlen(buffer) - sizeof(int), SEEK_CUR);
                fprintf(file, "%s %d\n", MARKER_REMOVIDO, rrn);
                printf("Chave %s removida\n", chave);
                fclose(file);
                return;
            }
        }
        printf("Chave %s não encontrada para remoção\n", chave);
        fclose(file);
    }
}

void inserir_registro(char *chave, char *nome_aluno, char *nome_disciplina, float media, float frequencia) {
    FILE *file = fopen("data.dat", "a");
    if (file) {
        fprintf(file, "%s#%s#%s#%.1f#%.1f\n", chave, nome_aluno, nome_disciplina, media, frequencia);
        fclose(file);
    }
}

void buscar_registro(char *chave) {
    FILE *file = fopen("data.dat", "r");
    char linha[200];

    if (file) {
        while (fgets(linha, sizeof(linha), file)) {
            if (strstr(linha, chave)) {
                printf("Registro encontrado: %s", linha);
                fclose(file);
                return;
            }
        }
        printf("Registro não encontrado.\n");
        fclose(file);
    }
}

void remover_registro(char *chave) {
    FILE *file = fopen("data.dat", "r");
    FILE *temp = fopen("temp.dat", "w");
    char linha[200];

    if (file && temp) {
        while (fgets(linha, sizeof(linha), file)) {
            if (!strstr(linha, chave)) {
                fputs(linha, temp);
            }
        }
        fclose(file);
        fclose(temp);
        remove("data.dat");
        rename("temp.dat", "data.dat");
    }
}