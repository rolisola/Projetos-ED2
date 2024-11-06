#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3
#define ORDEM_B 4 // Ordem da árvore-B, conforme especificado

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
    char nome_Aluno[MAX_NOME];
    char nome_Disciplina[MAX_NOME];
    float media;
    float frequencia;
} REGISTRO;

// Estrutura do nó da árvore-B
typedef struct BNode {
    int n;                  // Número de chaves no nó
    char chaves[ORDEM_B - 1][FIXO_ID + FIXO_SIGLA + 1]; // Chaves do nó
    struct BNode *filhos[ORDEM_B]; // Ponteiros para os filhos
    int folha;              // Indica se é uma folha
} BNode;

BNode *raiz = NULL;

// Funções auxiliares para manipulação da árvore-B
BNode* cria_no(int folha) {
    BNode *no = (BNode*)malloc(sizeof(BNode));
    no->n = 0;
    no->folha = folha;
    return no;
}

void inserir_no(BNode *no, char *chave, REGISTRO *registro, FILE *arquivo) {
    // Implementação da inserção de chave, incluindo:
    // - divisão de nó, se necessário
    // - promoção de chave
    // - mensagens de status de inserção
}

// Função para buscar um registro específico
REGISTRO* buscar_registro(char *chave) {
    // Implementação da busca na árvore-B para localizar o registro
    // Mensagens sobre a localização da chave
    return NULL; // Retorna NULL se não encontrar
}

// Função para listar todos os registros
void listar_todos(BNode *no) {
    // Percurso em-ordem na árvore-B
}

// Função para inserir um novo registro
void inserir(REGISTRO registro, FILE *arquivo) {
    char chave[FIXO_ID + FIXO_SIGLA + 1];
    snprintf(chave, sizeof(chave), "%s%s", registro.id_Aluno, registro.sigla_Disciplina);

    if (buscar_registro(chave) != NULL) {
        printf("Chave %s duplicada\n", chave);
    } else {
        inserir_no(raiz, chave, &registro, arquivo);
        printf("Chave %s inserida com sucesso\n", chave);
    }
}

// Função para carregar registros do arquivo insere.bin
void carregar_insercoes(FILE *arquivo) {
    REGISTRO registro;
    while (fread(&registro, sizeof(REGISTRO), 1, arquivo) == 1) {
        inserir(registro, arquivo);
    }
}

REGISTRO *carregar_insere(const char *nome_arquivo_insere) {
    // Carrega e retorna um vetor de registros do insere.bin
    FILE *arquivo_insere = abrir_arquivo(nome_arquivo_insere, "rb");
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

size_t contar_registros(const char *nome_arquivo) {
    // Conta a quantidade de registros existentes no arquivo
    FILE *arquivo = abrir_arquivo(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo.");
        exit(EXIT_FAILURE);
    }

    size_t tamanho_registro;
    if (strcmp(nome_arquivo, "insere.bin") == 0) {
        tamanho_registro = sizeof(REGISTRO);
    } /*else if (strcmp(nome_arquivo, "busca.bin") == 0) {
        tamanho_registro = sizeof(CHAVEPRIMARIA);
    } else if((strcmp(nome_arquivo, "busca_s.bin"){
         size_t tamanho_registro = sizeof(REGISTRO);
     }*/
    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo);

    if (tamanho_registro == 0) {
        fclose(arquivo);
        return 0; // Evita divisão por zero
    }

    size_t num_registros = tamanho_arquivo / tamanho_registro;

    fclose(arquivo);

    return num_registros;
}

int obter_auxiliar(int posicao) {
    // Obtem o valor de quantos registros já foram gravados
    // Possiveis valores de entrada: 0 (insere.bin), 4 (busca.bin)
    FILE *arquivo_auxiliar = abrir_criar_arquivo("auxiliar.bin", "rb");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    int valor = 0;

    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fread(&valor, sizeof(int), 1, arquivo_auxiliar);
    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fclose(arquivo_auxiliar);

    return valor;
}

void atualizar_auxiliar(int posicao, int valor) {
    // Atualiza valor de quantos registros foram usados do insere.bin, busca_p.bin e busca_s.bin
    FILE *arquivo_auxiliar = abrir_arquivo("auxiliar.bin", "rb+");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    fseek(arquivo_auxiliar, posicao, 0);
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    fclose(arquivo_auxiliar);
}

void imprime_vetor_insere(REGISTRO *vetor_insere, size_t tamanho_vetor_inserir) {
    for (int i = 0; i < tamanho_vetor_inserir; i++) {
        printf("Id Aluno:         (%s)\n", vetor_insere[i].id_Aluno);
        printf("Sigla Disciplina: (%s)\n", vetor_insere[i].sigla_Disciplina);
        printf("nome Aluno:       (%s)\n", vetor_insere[i].nome_Aluno);
        printf("Nome Disciplina:  (%s)\n", vetor_insere[i].nome_Disciplina);
        printf("média:            (%.1f)\n", vetor_insere[i].media);
        printf("frequência:       (%.1f)\n\n", vetor_insere[i].frequencia);
    }
}

// Função principal
int main() {
    int opcao;
    const char *nome_arquivo_dados = "dados.bin";
    const char *nome_arquivo_insere = "insere.bin";
    const char *nome_arquivo_busca = "busca.bin";

    REGISTRO *vetor_insere;
    vetor_insere = carregar_insere(nome_arquivo_insere);
    size_t tamanho_vetor_insere = contar_registros(nome_arquivo_insere);
    imprime_vetor_insere(vetor_insere, tamanho_vetor_insere);

    do {
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Listar todos\n");
        printf("3. Listar específico\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
/*
        switch (opcao) {
            case 1:

                //inserir(registro, arquivo);
                break;

            case 2:
                listar_todos(raiz);
                break;

            case 3:
                printf("ID do aluno e sigla da disciplina: ");
                //scanf("%s", registro.id_Aluno);
                strcat(registro.id_Aluno, registro.sigla_Disciplina);
                REGISTRO *result = buscar_registro(registro.id_Aluno);
                if (result != NULL) {
                    printf("Registro encontrado\n");
                    // Exibe os dados do registro encontrado
                } else {
                    printf("Registro não encontrado\n");
                }
                break;

            case 4:
                printf("Saindo...\n");
                break;

            default:
                printf("Opção inválida\n");
        }*/
    } while (opcao != 4);

    return 0;
}
