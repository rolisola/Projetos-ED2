#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3
#define MAX_VETOR 100

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
    char nome_Aluno[MAX_NOME];
    char nome_Disciplina[MAX_NOME];
    float media;
    float frequencia;
} REGISTRO;

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

// FUNÇÕES //
void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *nome_arquivo_indice_secundario, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere);
void buscar_por_chave_primaria(const char *nome_arquivo_dados, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria);
REGISTRO *carregar_insere(const char *nome_arquivo_insere);
size_t contar_registros(const char *nome_arquivo);
int obter_auxiliar(int posicao);
void atualizar_auxiliar(int posicao, int valor);
void imprime_vetor_insere(REGISTRO *vetor_insere, size_t tamanho_vetor_inserir);
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

int main() {
    int opcao;
    const char *nome_arquivo_dados = "dados.bin";
    const char *nome_arquivo_insere = "insere.bin";
    const char *nome_arquivo_busca_primaria = "busca_p.bin";
    const char *nome_arquivo_indice_primario = "indice_primario.bin";
    //const char *nome_arquivo_busca_secundaria = "busca_s.bin";
    const char *nome_arquivo_indice_secundario = "indice_secundario.bin";

    REGISTRO *vetor_insere;
    vetor_insere = carregar_insere(nome_arquivo_insere);
    size_t tamanho_vetor_insere = contar_registros(nome_arquivo_insere);

    CHAVEPRIMARIA *vetor_chave_primaria;
    vetor_chave_primaria = carregar_busca_primaria(nome_arquivo_busca_primaria);
    size_t tamanho_vetor_busca_primaria = contar_registros(nome_arquivo_busca_primaria);

    if (vetor_indice_primario == NULL) {
        // Inicializa o índice primário
        verificar_e_recriar_indice(nome_arquivo_dados, nome_arquivo_indice_primario);
        //carregar_indice_primario(nome_arquivo_dados, nome_arquivo_indice_primario);
    }
    // system("cls");

    do {
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Busca Primária\n");
        printf("3. Busca Secundária\n");
        printf("4. Sair e salvar\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            inserir_registro(nome_arquivo_dados, nome_arquivo_indice_primario, nome_arquivo_indice_secundario, vetor_insere, tamanho_vetor_insere);
            break;

        case 2:
            printf("Buscando.\n");
            buscar_por_chave_primaria(nome_arquivo_dados, vetor_chave_primaria, tamanho_vetor_busca_primaria);
            break;

        case 3:
            printf("Buscando.\n");
            break;
        case 4:
            printf("Salvando e saindo.\n");
            salvar_indice_primario(nome_arquivo_indice_primario);
            break;
        case 5:
            imprime_vetor_indice_primario();
            break;
        case 0:
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

void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *nome_arquivo_indice_secundario, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
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
    /*int nome_index = -1;
    for (int j = 0; j < tamanho_vetor_indice_secundario; j++) {
        if (strcmp(vetor_indice_secundario[j].nome, vetor_insere[posicao].nome_Aluno) == 0) {
            nome_index = j;
            break;
        }
    }

    if (nome_index == -1) {
        vetor_indice_secundario = realloc(vetor_indice_secundario, (tamanho_vetor_indice_secundario + 1) * sizeof(INDICESECUNDARIO));
        snprintf(vetor_indice_secundario[tamanho_vetor_indice_secundario].nome, MAX_NOME, "%s", vetor_insere[posicao].nome_Aluno);
        vetor_indice_secundario[tamanho_vetor_indice_secundario].offset = tamanho_lista_secundaria * sizeof(LISTAINDICESECUNDARIO);
        nome_index = tamanho_vetor_indice_secundario;
        tamanho_vetor_indice_secundario++;
    }

    lista_secundaria = realloc(lista_secundaria, (tamanho_lista_secundaria + 1) * sizeof(LISTAINDICESECUNDARIO));
    snprintf(lista_secundaria[tamanho_lista_secundaria].id_sigla, FIXO_ID + FIXO_SIGLA + 2, "%s%s", vetor_insere[posicao].id_Aluno, vetor_insere[posicao].sigla_Disciplina);
    lista_secundaria[tamanho_lista_secundaria].offset = -1;
    if (vetor_indice_secundario[nome_index].offset != tamanho_lista_secundaria * sizeof(LISTAINDICESECUNDARIO)) {
        int last_entry_index = vetor_indice_secundario[nome_index].offset / sizeof(LISTAINDICESECUNDARIO);
        while (lista_secundaria[last_entry_index].offset != -1) {
            last_entry_index = lista_secundaria[last_entry_index].offset / sizeof(LISTAINDICESECUNDARIO);
        }
        lista_secundaria[last_entry_index].offset = tamanho_lista_secundaria * sizeof(LISTAINDICESECUNDARIO);
    }

    tamanho_lista_secundaria++;*/

    fechar_arquivo(arquivo_dados);

    posicao++;
    atualizar_auxiliar(0, posicao);

    qsort(vetor_indice_primario, tamanho_vetor_indice_primario, sizeof(INDICEPRIMARIO), comparar_indice_primario);
    // salvar_indice_primario(nome_arquivo_indice_primario);
    //salvar_indice_secundario(nome_arquivo_indice_secundario);
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
    // printf("%ld\n", offset);
    if (offset != -1) {
        imprimir_registro_offset(nome_arquivo_dados, offset);
    } else {
        // imprimir_registro_offset(nome_arquivo_dados, offset);
        printf("Registro não encontrado.\n");
        return;
    }

    // Atualiza a posição para a próxima busca
    posicao++;
    atualizar_auxiliar(4, posicao);
}

// FUNÇÕES AUXILIARES //

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

    fechar_arquivo(arquivo_insere);

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
    } else if (strcmp(nome_arquivo, "busca_p.bin") == 0) {
        tamanho_registro = sizeof(CHAVEPRIMARIA);
    } /*else if((strcmp(nome_arquivo, "busca_s.bin"){
         size_t tamanho_registro = sizeof(REGISTRO);
     }*/
    size_t tamanho_arquivo = obter_tamanho_arquivo(arquivo);

    if (tamanho_registro == 0) {
        fechar_arquivo(arquivo);
        return 0; // Evita divisão por zero
    }

    size_t num_registros = tamanho_arquivo / tamanho_registro;

    fechar_arquivo(arquivo);

    return num_registros;
}

int obter_auxiliar(int posicao) {
    // Obtem o valor de quantos registros já foram gravados
    // Possiveis valores de entrada: 0 (insere.bin), 4 (busca_p.bin), 8 (busca_s.bin)
    FILE *arquivo_auxiliar = abrir_criar_arquivo("auxiliar.bin", "rb");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    int valor = 0;

    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fread(&valor, sizeof(int), 1, arquivo_auxiliar);
    fseek(arquivo_auxiliar, posicao, SEEK_SET);
    fechar_arquivo(arquivo_auxiliar);

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
    fechar_arquivo(arquivo_auxiliar);
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
    fechar_arquivo(arquivo_indice);
}

void recriar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario) {
    // Recria o índice lendo os registros do 'dados.bin'
    FILE *arquivo_dados = abrir_arquivo(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        perror("Arquivo de dados não encontrado. Não é possível recriar índice");
        return;
    }

    tamanho_vetor_indice_primario = obter_auxiliar(0);
    // vetor_indice_primario = realloc(sizeof(INDICEPRIMARIO) * tamanho_vetor_indice_primario);
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
        sscanf(string_formatada, "%3s#%3s", vetor_indice_primario[i].id_Aluno, vetor_indice_primario[i].sigla_Disciplina);
    }

    fechar_arquivo(arquivo_dados);

    keysort();
}

/*void salvar_indice_secundario(const char *nome_arquivo_indice_secundaria) {
    FILE *arquivo_indice_secundario = abrir_arquivo(nome_arquivo_indice_secundaria, "wb");
    if (!arquivo_indice_secundario) {
        perror("Erro ao abrir o arquivo de índice secundário");
        return;
    }

    fwrite(vetor_indice_secundario, sizeof(INDICESECUNDARIO), tamanho_vetor_indice_secundario, arquivo_indice_secundario);
    fwrite(lista_secundaria, sizeof(LISTAINDICESECUNDARIO), tamanho_lista_secundaria, arquivo_indice_secundario);
    fechar_arquivo(arquivo_indice_secundario);
}*/

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

    fechar_arquivo(arquivo_busca_primaria);

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
    // string_formatada[tamanho_string] = '\0';
    printf("Registro encontrado: %s\n", string_formatada);
    fechar_arquivo(arquivo_dados);
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
        fechar_arquivo(arquivo_indice);
        // Carregar índices do arquivo (implementar conforme necessário)
    }
}

void inicializar_vetor_indice() {
    // Inicializa o vetor de índices
    vetor_indice_primario = malloc(sizeof(INDICEPRIMARIO) * 100); // Exemplo de alocação
    tamanho_vetor_indice_primario = 0;
}
