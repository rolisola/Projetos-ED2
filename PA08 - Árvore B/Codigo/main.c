#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3
#define MAX_VETOR 100
#define ORDEM_ARVORE_B 4 // Ordem da árvore-B, conforme especificado no enunciado

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
    char nome_Aluno[MAX_NOME];
    char nome_Disciplina[MAX_NOME];
    float media;
    float frequencia;
} REGISTRO;

// Estrutura do nó da árvore-B
typedef struct NO_ARVORE_B {
    int n;                  // Número de chaves no nó
    char chaves[ORDEM_ARVORE_B - 1][FIXO_ID + FIXO_SIGLA + 1]; // Chaves do nó
    struct NO_ARVORE_B *filhos[ORDEM_ARVORE_B]; // Ponteiros para os filhos
    int folha;              // Indica se é uma folha
} NO_ARVORE_B;

NO_ARVORE_B *raiz = NULL;

// Funções auxiliares para manipulação da árvore-B
NO_ARVORE_B* cria_no(int folha) {
    NO_ARVORE_B *no = (NO_ARVORE_B*)malloc(sizeof(NO_ARVORE_B));
    no->n = 0;
    no->folha = folha;
    return no;
}

typedef struct {
    int num_chaves;
    int chaves[ORDEM_ARVORE_B - 1];
    long filhos[ORDEM_ARVORE_B];
    long posicoes[ORDEM_ARVORE_B - 1];
    int eh_folha;
} NoArvoreB;

typedef struct {
    FILE *arquivo_dados;
    FILE *arquivo_arvore;
    long raiz;
} ArvoreB;

void inicializa_arvore(ArvoreB *arvore, const char *nome_arquivo_dados, const char *nome_arquivo_arvore) {
    arvore->arquivo_dados = fopen(nome_arquivo_dados, "r+b");
    arvore->arquivo_arvore = fopen(nome_arquivo_arvore, "r+b");

    if (!arvore->arquivo_dados) {
        arvore->arquivo_dados = fopen(nome_arquivo_dados, "w+b");
    }
    if (!arvore->arquivo_arvore) {
        arvore->arquivo_arvore = fopen(nome_arquivo_arvore, "w+b");
        NoArvoreB raiz = {0, {0}, {0}, {0}, 1};
        fseek(arvore->arquivo_arvore, 0, SEEK_SET);
        fwrite(&raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
        arvore->raiz = 0;
    } else {
        fseek(arvore->arquivo_arvore, 0, SEEK_SET);
        NoArvoreB raiz;
        fread(&raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
        arvore->raiz = 0;
    }
}

void busca_arvore_b(ArvoreB *arvore, int chave, NoArvoreB *no, int *pos) {
    int i = 0;
    while (i < no->num_chaves && chave > no->chaves[i]) {
        i++;
    }
    if (i < no->num_chaves && chave == no->chaves[i]) {
        *pos = i;
        return;
    }
    if (no->eh_folha) {
        *pos = -1;
        return;
    } else {
        fseek(arvore->arquivo_arvore, no->filhos[i] * sizeof(NoArvoreB), SEEK_SET);
        NoArvoreB filho;
        fread(&filho, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
        busca_arvore_b(arvore, chave, &filho, pos);
    }
}

void divide_no(ArvoreB *arvore, NoArvoreB *x, int i, NoArvoreB *y) {
    NoArvoreB z;
    z.eh_folha = y->eh_folha;
    z.num_chaves = (ORDEM_ARVORE_B / 2) - 1;

    for (int j = 0; j < (ORDEM_ARVORE_B / 2) - 1; j++) {
        z.chaves[j] = y->chaves[j + ORDEM_ARVORE_B / 2];
    }
    if (!y->eh_folha) {
        for (int j = 0; j < ORDEM_ARVORE_B / 2; j++) {
            z.filhos[j] = y->filhos[j + ORDEM_ARVORE_B / 2];
        }
    }
    y->num_chaves = (ORDEM_ARVORE_B / 2) - 1;

    for (int j = x->num_chaves; j >= i + 1; j--) {
        x->filhos[j + 1] = x->filhos[j];
    }
    x->filhos[i + 1] = ftell(arvore->arquivo_arvore) / sizeof(NoArvoreB);
    fseek(arvore->arquivo_arvore, 0, SEEK_END);
    fwrite(&z, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);

    for (int j = x->num_chaves - 1; j >= i; j--) {
        x->chaves[j + 1] = x->chaves[j];
    }
    x->chaves[i] = y->chaves[ORDEM_ARVORE_B / 2 - 1];
    x->num_chaves++;
}

void insere_nao_cheio(ArvoreB *arvore, NoArvoreB *no, int chave, long posicao) {
    int i = no->num_chaves - 1;
    if (no->eh_folha) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            no->posicoes[i + 1] = no->posicoes[i];
            i--;
        }
        no->chaves[i + 1] = chave;
        no->posicoes[i + 1] = posicao;
        no->num_chaves++;
        fseek(arvore->arquivo_arvore, ftell(arvore->arquivo_arvore) - sizeof(NoArvoreB), SEEK_SET);
        fwrite(no, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            i--;
        }
        i++;
        fseek(arvore->arquivo_arvore, no->filhos[i] * sizeof(NoArvoreB), SEEK_SET);
        NoArvoreB filho;
        fread(&filho, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
        if (filho.num_chaves == ORDEM_ARVORE_B - 1) {
            divide_no(arvore, no, i, &filho);
            if (chave > no->chaves[i]) {
                i++;
            }
        }
        insere_nao_cheio(arvore, &filho, chave, posicao);
    }
}

void insere_arvore_b(ArvoreB *arvore, int chave, REGISTRO *registro) {
    fseek(arvore->arquivo_dados, 0, SEEK_END);
    long posicao = ftell(arvore->arquivo_dados);
    fwrite(registro, sizeof(REGISTRO), 1, arvore->arquivo_dados);

    fseek(arvore->arquivo_arvore, arvore->raiz * sizeof(NoArvoreB), SEEK_SET);
    NoArvoreB raiz;
    fread(&raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
    if (raiz.num_chaves == ORDEM_ARVORE_B - 1) {
        NoArvoreB nova_raiz = {0, {0}, {0}, {0}, 0};
        nova_raiz.filhos[0] = arvore->raiz;
        divide_no(arvore, &nova_raiz, 0, &raiz);
        insere_nao_cheio(arvore, &nova_raiz, chave, posicao);
        fseek(arvore->arquivo_arvore, 0, SEEK_END);
        arvore->raiz = ftell(arvore->arquivo_arvore) / sizeof(NoArvoreB) - 1;
        fseek(arvore->arquivo_arvore, arvore->raiz * sizeof(NoArvoreB), SEEK_SET);
        fwrite(&nova_raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
    } else {
        insere_nao_cheio(arvore, &raiz, chave, posicao);
    }
}

void percorre_em_ordem(ArvoreB *arvore, NoArvoreB *no) {
    int i;
    for (i = 0; i < no->num_chaves; i++) {
        if (!no->eh_folha) {
            fseek(arvore->arquivo_arvore, no->filhos[i] * sizeof(NoArvoreB), SEEK_SET);
            NoArvoreB filho;
            fread(&filho, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
            percorre_em_ordem(arvore, &filho);
        }
        fseek(arvore->arquivo_dados, no->posicoes[i], SEEK_SET);
        REGISTRO registro;
        fread(&registro, sizeof(REGISTRO), 1, arvore->arquivo_dados);
        printf("ID: %d, Sigla: %s, Nome: %s, Disciplina: %s, Media: %.2f, Frequencia: %.2f\n",
               registro.id_Aluno, registro.sigla_Disciplina, registro.nome_Aluno, registro.nome_Disciplina, registro.media, registro.frequencia);
    }
    if (!no->eh_folha) {
        fseek(arvore->arquivo_arvore, no->filhos[i] * sizeof(NoArvoreB), SEEK_SET);
        NoArvoreB filho;
        fread(&filho, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
        percorre_em_ordem(arvore, &filho);
    }
}

void lista_todos_registros(ArvoreB *arvore) {
    fseek(arvore->arquivo_arvore, arvore->raiz * sizeof(NoArvoreB), SEEK_SET);
    NoArvoreB raiz;
    fread(&raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
    percorre_em_ordem(arvore, &raiz);
}


void lista_registro_especifico(ArvoreB *arvore, int id, const char *sigla) {
    int chave = id * 1000 + atoi(sigla);
    fseek(arvore->arquivo_arvore, arvore->raiz * sizeof(NoArvoreB), SEEK_SET);
    NoArvoreB raiz;
    fread(&raiz, sizeof(NoArvoreB), 1, arvore->arquivo_arvore);
    int pos;
    busca_arvore_b(arvore, chave, &raiz, &pos);
    if (pos != -1) {
        fseek(arvore->arquivo_dados, raiz.posicoes[pos], SEEK_SET);
        REGISTRO registro;
        fread(&registro, sizeof(REGISTRO), 1, arvore->arquivo_dados);
        printf("ID: %d, Sigla: %s, Nome: %s, Disciplina: %s, Media: %.2f, Frequencia: %.2f\n",
               registro.id_Aluno, registro.sigla_Disciplina, registro.nome_Aluno, registro.nome_Disciplina, registro.media, registro.frequencia);
    } else {
        printf("Chave %d não encontrada\n", chave);
    }
}

void insere_registro(ArvoreB *arvore, REGISTRO *vetor_insere, size_t tamanho_vetor_insere, const char *nome_arquivo_dados) {

    int posicao = obter_auxiliar(0);
    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }
    printf("Inserindo registro: %d\n", posicao + 1);

    // Converter a estrutura REGISTRO em uma string no formato especificado
    char string[256]; // Assumindo que o tamanho máximo do registro será menor que 256 bytes
    int tamanho_vetor_indice = snprintf(string, sizeof(string), "%s#%s#%s#%s#%.1f#%.1f",
                                        vetor_insere[posicao].id_Aluno, vetor_insere[posicao].sigla_Disciplina, vetor_insere[posicao].nome_Aluno, vetor_insere[posicao].nome_Disciplina,
                                        vetor_insere[posicao].media, vetor_insere[posicao].frequencia);

    // Escrever o registro no arquivo de dados
    FILE *arquivo_dados = abrir_criar_arquivo(nome_arquivo_dados, "ab");
    if (arquivo_dados == NULL) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }
    fwrite(&tamanho_vetor_indice, sizeof(int), 1, arquivo_dados);
    long offset = ftell(arquivo_dados);
    fwrite(string, sizeof(char), tamanho_vetor_indice, arquivo_dados);

    REGISTRO registro;
    printf("ID do aluno: ");
    scanf("%d", &registro.id_Aluno);
    printf("Sigla da disciplina: ");
    scanf("%s", registro.sigla_Disciplina);
    printf("Nome do aluno: ");
    getchar();  // Limpar o buffer do teclado
    fgets(registro.nome_Aluno, 51, stdin);
    strtok(registro.nome_Aluno, "\n");  // Remover o caractere de nova linha
    printf("Nome da disciplina: ");
    fgets(registro.nome_Disciplina, 51, stdin);
    strtok(registro.nome_Disciplina, "\n");  // Remover o caractere de nova linha
    printf("Média: ");
    scanf("%f", &registro.media);
    printf("Frequência: ");
    scanf("%f", &registro.frequencia);

    int chave = atoi(registro.id_Aluno) * 1000 + atoi(registro.sigla_Disciplina);  // Compor a chave primária
    insere_arvore_b(arvore, chave, &registro);

    // Atualiza a posição para a próxima inserção
    posicao++;
    atualizar_auxiliar(0, posicao);
}










/*
typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
    long offset; // Deslocamento no arquivo de dados
} INDICEPRIMARIO;

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
} CHAVEPRIMARIA;

INDICEPRIMARIO *vetor_indice_primario = NULL;
// SecondaryINDICEPRIMARIO *secondary_index = NULL;
// qSecondaryListEntry *secondary_list = NULL;
int tamanho_vetor_indice_primario = 0;
// int secondary_index_size = 0;
// int secondary_list_size = 0;
*/






// FUNÇÕES //

REGISTRO *carregar_insere(const char *nome_arquivo_insere);
int obter_auxiliar(int posicao);
void atualizar_auxiliar(int posicao, int valor);
void imprime_vetor_insere(REGISTRO *vetor_insere, size_t tamanho_vetor_inserir);
size_t contar_registros(const char *nome_arquivo);





/*
void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *secondary_index_filename, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere);
void buscar_por_chave_primaria(const char *nome_arquivo_dados, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria);
void carregar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice);
void salvar_indice_primario(const char *nome_arquivo_indice_primario);
CHAVEPRIMARIA *carregar_busca_primaria(const char *nome_arquivo_busca_primaria);
void imprime_vetor_chave_primaria(CHAVEPRIMARIA *vetor_chave_primaria, int tamanho_vetor_chave_primaria);
int comparar_indice_primario(const void *a, const void *b);
void keysort();
long busca_binaria(CHAVEPRIMARIA chave_primaria);
void recriar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario);
void imprimir_registro_offset(const char *nome_arquivo_dados, long offset);
void imprime_vetor_indice_primario();
void inicializar_vetor_indice();
void verificar_e_recriar_indice(const char *nome_arquivo_dados, const char *nome_arquivo_indice);
*/
int main() {
    int opcao;
    const char *nome_arquivo_dados = "dados.bin";
    const char *nome_arquivo_arvore = "arvore.bin";
    const char *nome_arquivo_insere = "insere.bin";
    // const char *nome_arquivo_busca_primaria = "busca_p.bin";
    // const char *nome_arquivo_indice_primario = "indice_primario.bin";
    // const char *nome_arquivo_busca_secundaria = "busca_s.bin";
    // const char *secondary_index_filename = "indice_secundario.bin";

    REGISTRO *vetor_insere;
    vetor_insere = carregar_insere(nome_arquivo_insere);
    size_t tamanho_vetor_insere = contar_registros(nome_arquivo_insere);

    ArvoreB arvore;
    inicializa_arvore(&arvore, nome_arquivo_dados, nome_arquivo_arvore);

/*
    //CHAVEPRIMARIA *vetor_chave_primaria;
    //vetor_chave_primaria = carregar_busca_primaria(nome_arquivo_busca_primaria);
    //size_t tamanho_vetor_busca_primaria = contar_registros(nome_arquivo_busca_primaria);

    if (vetor_indice_primario == NULL) {
        // Inicializa o índice primário
        verificar_e_recriar_indice(nome_arquivo_dados, nome_arquivo_indice_primario);
        carregar_indice_primario(nome_arquivo_dados, nome_arquivo_indice_primario);
    }
    */
    system("cls");

    do {
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Listar dados de todos os alunos\n");
        printf("3. Listar dados de um aluno numa disciplina\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            //inserir_registro(nome_arquivo_dados, nome_arquivo_indice_primario, secondary_index_filename, vetor_insere, tamanho_vetor_insere);
            break;

        case 2:
            printf("Buscando.\n");
            //buscar_por_chave_primaria(nome_arquivo_dados, vetor_chave_primaria, tamanho_vetor_busca_primaria);
            break;

        case 3:
            printf("Buscando.\n");
            break;
        case 4:
            //salvar_indice_primario(nome_arquivo_indice_primario);
            break;
        case 5:
            imprime_vetor_insere(vetor_insere, tamanho_vetor_insere);
            break;
        case 0:
            //salvar_indice_primario(nome_arquivo_indice_primario);
            printf("Salvando e saindo.\n");
            printf("Programa finalizado!\n");
            break;

        default:
            printf("Opção inválida, tente novamente!\n\n\n");
            break;
        }
    } while (!((opcao == 0) || (opcao == 4)));
    return 0;
}

// FUNÇÕES PRINCIPAIS //

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
    // Atualiza valor de quantos registros foram usados do insere.bin, busca.bin
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
    }
    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo);

    if (tamanho_registro == 0) {
        fclose(arquivo);
        return 0; // Evita divisão por zero
    }

    size_t num_registros = tamanho_arquivo / tamanho_registro;

    fclose(arquivo);

    return num_registros;
}










































/*
void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *secondary_index_filename, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    // Insere o registro do vetor no dados.bin
    int posicao = obter_auxiliar(0);
    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }
    printf("Inserindo registro: %d\n", posicao + 1);

    FILE *arquivo_dados = abrir_criar_arquivo(nome_arquivo_dados, "ab");
    if (arquivo_dados == NULL) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }

    // Converter a estrutura REGISTRO em uma string no formato especificado
    char string[256]; // Assumindo que o tamanho máximo do registro será menor que 256 bytes
    int tamanho_vetor_indice = snprintf(string, sizeof(string), "%s#%s#%s#%s#%.1f#%.1f",
                                        vetor_insere[posicao].id_Aluno, vetor_insere[posicao].sigla_Disciplina, vetor_insere[posicao].nome_Aluno, vetor_insere[posicao].nome_Disciplina,
                                        vetor_insere[posicao].media, vetor_insere[posicao].frequencia);

    // Escrever o registro no arquivo de dados
    fwrite(&tamanho_vetor_indice, sizeof(int), 1, arquivo_dados);
    long offset = ftell(arquivo_dados);
    fwrite(string, sizeof(char), tamanho_vetor_indice, arquivo_dados);

    // Atualizar índice primário
    vetor_indice_primario = realloc(vetor_indice_primario, (tamanho_vetor_indice_primario + 1) * sizeof(INDICEPRIMARIO));
    snprintf(vetor_indice_primario[tamanho_vetor_indice_primario].id_Aluno, FIXO_ID + 1, "%s", vetor_insere[posicao].id_Aluno);
    snprintf(vetor_indice_primario[tamanho_vetor_indice_primario].sigla_Disciplina, FIXO_SIGLA + 1, "%s", vetor_insere[posicao].sigla_Disciplina);
    vetor_indice_primario[tamanho_vetor_indice_primario].offset = offset;
    tamanho_vetor_indice_primario++;

    // Atualizar índice secundário
    int nome_index = -1;
    for (int j = 0; j < secondary_index_size; j++) {
        if (strcmp(secondary_index[j].nome, vetor_insere[posicao].nome_Aluno) == 0) {
            nome_index = j;
            break;
        }
    }

    if (nome_index == -1) {
        secondary_index = realloc(secondary_index, (secondary_index_size + 1) * sizeof(SecondaryINDICEPRIMARIO));
        snprintf(secondary_index[secondary_index_size].nome, MAX_NOME, "%s", vetor_insere[posicao].nome_Aluno);
        secondary_index[secondary_index_size].offset = secondary_list_size * sizeof(SecondaryListEntry);
        nome_index = secondary_index_size;
        secondary_index_size++;
    }

    secondary_list = realloc(secondary_list, (secondary_list_size + 1) * sizeof(SecondaryListEntry));
    snprintf(secondary_list[secondary_list_size].id_sigla, FIXO_ID + FIXO_SIGLA + 2, "%s%s", vetor_insere[posicao].id_Aluno, vetor_insere[posicao].sigla_Disciplina);
    secondary_list[secondary_list_size].offset = -1;
    if (secondary_index[nome_index].offset != secondary_list_size * sizeof(SecondaryListEntry)) {
        int last_entry_index = secondary_index[nome_index].offset / sizeof(SecondaryListEntry);
        while (secondary_list[last_entry_index].offset != -1) {
            last_entry_index = secondary_list[last_entry_index].offset / sizeof(SecondaryListEntry);
        }
        secondary_list[last_entry_index].offset = secondary_list_size * sizeof(SecondaryListEntry);
    }

    secondary_list_size++;

    fclose(arquivo_dados);

    posicao++;
    atualizar_auxiliar(0, posicao);

    qsort(vetor_indice_primario, tamanho_vetor_indice_primario, sizeof(INDICEPRIMARIO), comparar_indice_primario);
    // salvar_indice_primario(nome_arquivo_indice_primario);
    // save_secondary_index(secondary_index_filename);
}

void buscar_por_chave_primaria(const char *nome_arquivo_dados, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria) {
    // Obtém a posição da próxima busca
    int posicao = obter_auxiliar(4);
    if (posicao >= tamanho_vetor_busca_primaria) {
        printf("Todas as buscas por chave primaria ja foram realizadas!\n");
        return;
    }
    if (tamanho_vetor_indice_primario == 0) {
        printf("Nenhum registro foi inserido ainda.\n");
        return;
    }

    // Obter a chave primária atual
    CHAVEPRIMARIA chave_primaria = vetor_busca_primaria[posicao];
    printf("Buscando chave: %s-%s\n", chave_primaria.id_Aluno, chave_primaria.sigla_Disciplina);

    long offset = busca_binaria(chave_primaria);
    //printf("%ld\n", offset);
    if (offset != -1) {
        imprimir_registro_offset(nome_arquivo_dados, offset);
    } else {
        //imprimir_registro_offset(nome_arquivo_dados, offset);
        printf("Registro não encontrado.\n");
    }

    // Atualiza a posição para a próxima busca
    posicao++;
    atualizar_auxiliar(4, posicao);
}

// FUNÇÕES AUXILIARES //







void carregar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice) {
    // Carrega o índice do arquivo 'indice_primario.bin'
    FILE *arquivo_indice_primario = abrir_arquivo(nome_arquivo_indice, "rb");
    if (arquivo_indice_primario == NULL) {
        perror("Arquivo de indice primário não encontrado, recriando índice...\n");
        recriar_indice_primario(nome_arquivo_dados, nome_arquivo_indice);
    }

    // Lê todo o o arquivo de índice primário
    vetor_indice_primario = malloc(sizeof(INDICEPRIMARIO) * MAX_VETOR);
    if (vetor_indice_primario == NULL) {
        perror("Erro ao alocar memória para vetor_indice_primario");
        exit(EXIT_FAILURE);
    }
    while (fread(&vetor_indice_primario[tamanho_vetor_indice_primario], sizeof(INDICEPRIMARIO), 1, arquivo_indice_primario)) {
        tamanho_vetor_indice_primario++;
    }
    if (tamanho_vetor_indice_primario != obter_auxiliar(0)) {
        recriar_indice_primario(nome_arquivo_dados, nome_arquivo_indice);
    }
    fechar_arquivo(arquivo_indice_primario);

    // Ordena o índice por keysort
    keysort();
}

void salvar_indice_primario(const char *nome_arquivo_indice_primario) {
    FILE *arquivo_indice = abrir_arquivo(nome_arquivo_indice_primario, "wb");
    if (arquivo_indice == NULL) {
        perror("Erro ao abrir o arquivo de índice primário");
        return;
    }

    fwrite(vetor_indice_primario, sizeof(INDICEPRIMARIO), tamanho_vetor_indice_primario, arquivo_indice);
    fclose(arquivo_indice);
}

void recriar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario) {
    // Recria o índice lendo os registros do 'dados.bin'
    FILE *arquivo_dados = abrir_arquivo(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        perror("Arquivo de dados não encontrado. Não é possível recriar índice");
        return;
    }

    tamanho_vetor_indice_primario = obter_auxiliar(0);
    //vetor_indice_primario = realloc(sizeof(INDICEPRIMARIO) * tamanho_vetor_indice_primario);
    if (vetor_indice_primario == NULL) {
        perror("Erro ao alocar memória para vetor_indice_primario");
        fechar_arquivo(arquivo_dados);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tamanho_vetor_indice_primario; i++) {
        int tamanho_string;
        char string_formatada[256];
        vetor_indice_primario[i].offset = ftell(arquivo_dados);
        fread(&tamanho_string, sizeof(long), 1, arquivo_dados);
        fread(string_formatada, sizeof(char), tamanho_string, arquivo_dados);
        // printf("tamanho string: %d\n", tamanho_string);
        // printf("string: %s\n", string_formatada);
        sscanf(string_formatada, "%3s#%3s", vetor_indice_primario[i].id_Aluno, vetor_indice_primario[i].sigla_Disciplina);
        // printf("idaluno: %s\n", vetor_indice_primario[i].id_Aluno);
        // printf("sigla: %s\n",vetor_indice_primario[i].sigla_Disciplina);
        // printf("offset: %ld\n",vetor_indice_primario[i].offset);
    }

    while (fread(&offset, sizeof(long), 1, arquivo_dados)) {
        printf("offset: %ld\n", offset);
        char string_formatada[256];
        int tamanho_string = 0;

        fread(&tamanho_string, sizeof(int), 1, arquivo_dados);


        vetor_indice_primario[tamanho_vetor_indice_primario].offset = offset;
        printf("a: %s\n",vetor_indice_primario[ta])
        tamanho_vetor_indice_primario++;
    }

    fechar_arquivo(arquivo_dados);

    keysort();
    // salvar_indice_primario(nome_arquivo_indice_primario);
}

void save_secondary_index(const char *index_filename) {
    FILE *index_file = fopen(index_filename, "wb");
    if (!index_file) {
        perror("Erro ao abrir o arquivo de índice secundário");
        return;
    }

    fwrite(secondary_index, sizeof(SecondaryINDICEPRIMARIO), secondary_index_size, index_file);
    fwrite(secondary_list, sizeof(SecondaryListEntry), secondary_list_size, index_file);
    fclose(index_file);
}

CHAVEPRIMARIA *carregar_busca_primaria(const char *nome_arquivo_busca_primaria) {
    // Carrega e retorna um vetor de registros do insere.bin
    FILE *arquivo_busca_primaria = abrir_arquivo(nome_arquivo_busca_primaria, "rb");
    if (arquivo_busca_primaria == NULL) {
        perror("Erro ao abrir arquivo.");
        exit(EXIT_FAILURE);
    }

    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo_busca_primaria);
    size_t quantidade_registros = tamanho_arquivo / sizeof(CHAVEPRIMARIA);

    CHAVEPRIMARIA *vetor_busca_primaria = (CHAVEPRIMARIA *)malloc(quantidade_registros * sizeof(CHAVEPRIMARIA));
    if (vetor_busca_primaria == NULL) {
        perror("Erro ao alocar memória.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < quantidade_registros; i++) {
        fread(&vetor_busca_primaria[i], sizeof(CHAVEPRIMARIA), 1, arquivo_busca_primaria);
    }

    fclose(arquivo_busca_primaria);

    return vetor_busca_primaria;
}

void imprime_vetor_chave_primaria(CHAVEPRIMARIA *vetor_chave_primaria, int tamanho_vetor_chave_primaria) {
    for (int i = 0; i < tamanho_vetor_chave_primaria; i++) {
        printf("%d\n", i);
        printf("Id Aluno:         (%s)\n", vetor_chave_primaria[i].id_Aluno);
        printf("Sigla Disciplina: (%s)\n\n", vetor_chave_primaria[i].sigla_Disciplina);
    }
}

void imprime_vetor_indice_primario() {
    for (int i = 0; i < tamanho_vetor_indice_primario; i++) {
        printf("%d\n", i);
        printf("Id Aluno:         (%s)\n", vetor_indice_primario[i].id_Aluno);
        printf("Sigla Disciplina: (%s)\n", vetor_indice_primario[i].sigla_Disciplina);
        printf("Offset: (%ld)\n\n", vetor_indice_primario[i].offset);
    }
}

int comparar_indice_primario(const void *a, const void *b) {
    // Função de comparação para ordenar e buscar usando keysorting
    INDICEPRIMARIO *ia = (INDICEPRIMARIO *)a;
    INDICEPRIMARIO *ib = (INDICEPRIMARIO *)b;

    int resultado_id = strncmp(ia->id_Aluno, ib->id_Aluno, 4);
    if (resultado_id == 0)
        return strncmp(ia->sigla_Disciplina, ib->sigla_Disciplina, 4);

    return resultado_id;
}

void keysort() {
    // Ordena o vetor de indice por chave primarioa(id_aluno + sigla_disciplina)
    qsort(vetor_indice_primario, tamanho_vetor_indice_primario, sizeof(INDICEPRIMARIO), comparar_indice_primario);
}

long busca_binaria(CHAVEPRIMARIA chave_primaria) {
    // Realiza a busca binária de um registro no indice com base na chave primária
    int esquerda = 0, direita = tamanho_vetor_indice_primario - 1;

    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        int cmp_id = strcmp(chave_primaria.id_Aluno, vetor_indice_primario[meio].id_Aluno);
        if (cmp_id == 0) {
            int cmp_code = strcmp(chave_primaria.sigla_Disciplina, vetor_indice_primario[meio].sigla_Disciplina);
            if (cmp_code == 0) {
                return vetor_indice_primario[meio].offset; // Encontrou o registro
            } else if (cmp_code < 0) {
                direita = meio - 1;
            } else {
                esquerda = meio + 1;
            }
        } else if (cmp_id < 0) {
            direita = meio - 1;
        } else {
            esquerda = meio + 1;
        }
    }

    return -1; // Registro não encontrado
}

void imprimir_registro_offset(const char *nome_arquivo_dados, long offset) {
    // Função para imprimir registro do 'dados.bin' dado um offset
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        perror("Erro ao abrir arquivo de dados");
        return;
    }
    // printf("offset %ld\n", offset);
    fseek(arquivo_dados, offset, SEEK_SET);
    int tamanho_string;
    fread(&tamanho_string, sizeof(int), 1, arquivo_dados);
    // printf("tamanho string: %d\n", tamanho_string);
    char string_formatada[256];
    fread(string_formatada, sizeof(char), tamanho_string, arquivo_dados);
    //string_formatada[tamanho_string] = '\0';
    printf("Registro encontrado: %s\n", string_formatada);
    fclose(arquivo_dados);
}

void verificar_e_recriar_indice(const char *nome_arquivo_dados, const char *nome_arquivo_indice) {
    FILE *arquivo_indice = fopen(nome_arquivo_indice, "r");
    if (arquivo_indice == NULL) {
        // Arquivo de índice não existe, recriar índice
        printf("Arquivo de índice não encontrado. Recriando índice...\n");
        inicializar_vetor_indice();
        recriar_indice_primario(nome_arquivo_dados, nome_arquivo_indice);
    } else {
        // Arquivo de índice existe, carregar índices
        fclose(arquivo_indice);
        // Carregar índices do arquivo (implementar conforme necessário)
    }
}

void inicializar_vetor_indice() {
    // Inicializa o vetor de índices
    vetor_indice_primario = malloc(sizeof(INDICEPRIMARIO) * 100); // Exemplo de alocação
    tamanho_vetor_indice_primario = 0;
}
*/
