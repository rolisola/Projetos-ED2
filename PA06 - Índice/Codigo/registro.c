#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "arquivo.h"

INDICEPRIMARIO *vetor_indice_primario = NULL;
// SecondaryINDICEPRIMARIO *secondary_index = NULL;
// qSecondaryListEntry *secondary_list = NULL;
int tamanho_vetor_indice_primario = 0;
// int secondary_index_size = 0;
// int secondary_list_size = 0;

// FUNÇÕES PRINCIPAIS //

void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *secondary_index_filename, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere) {
    // Insere o registro do vetor no historico.bin
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
    // printf("tamanho_vetor_indice_primario %d\n", tamanho_vetor_indice_primario);

    // Atualizar índice secundário
    /*int nome_index = -1;
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

    secondary_list_size++;*/

    fclose(arquivo_dados);

    posicao++;
    atualizar_auxiliar(0, posicao);

    qsort(vetor_indice_primario, tamanho_vetor_indice_primario, sizeof(INDICEPRIMARIO), comparar_indice_primario);
    salvar_indice_primario(nome_arquivo_indice_primario);
    // save_secondary_index(secondary_index_filename);
}

void buscar_por_chave_primaria(const char *nome_arquivo_dados, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria) {
    // Obtém a posição da próxima busca
    int posicao = obter_auxiliar(4);
    if (posicao >= tamanho_vetor_busca_primaria) {
        printf("Todas as buscas por chave primaria ja foram realizadas!\n");
        return;
    }

    // Obter a chave primária atual
    CHAVEPRIMARIA chave_primaria = vetor_busca_primaria[posicao];
    printf("Buscando chave: %s-%s\n", chave_primaria.id_Aluno, chave_primaria.sigla_Disciplina);

    long offset = busca_binaria(chave_primaria);
    if (offset != 0) {
        imprimir_registro_offset(nome_arquivo_dados, offset);
    } else {
        printf("Registro não encontrado.\n");
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
    } else if (strcmp(nome_arquivo, "busca_p.bin") == 0) {
        tamanho_registro = sizeof(CHAVEPRIMARIA);
    } /*else if((strcmp(nome_arquivo, "busca_s.bin"){
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

void carregar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice) {
    // Carrega o índice do arquivo 'indice_primario.bin'
    FILE *arquivo_indice_primario = fopen(nome_arquivo_indice, "rb");
    if (arquivo_indice_primario == NULL) {
        perror("Arquivo de indice primário não encontrado, recriando índice...\n");
        recriar_indice_primario(nome_arquivo_dados, nome_arquivo_indice);
        return;
    }

    // Lê todo o o arquivo de índice primário
    while (fread(&vetor_indice_primario[tamanho_vetor_indice_primario], sizeof(INDICEPRIMARIO), 1, arquivo_indice_primario)) {
        tamanho_vetor_indice_primario++;
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

/*void save_secondary_index(const char *index_filename) {
    FILE *index_file = fopen(index_filename, "wb");
    if (!index_file) {
        perror("Erro ao abrir o arquivo de índice secundário");
        return;
    }

    fwrite(secondary_index, sizeof(SecondaryINDICEPRIMARIO), secondary_index_size, index_file);
    fwrite(secondary_list, sizeof(SecondaryListEntry), secondary_list_size, index_file);
    fclose(index_file);
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

void recriar_indice_primario(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario) {
    // Recria o índice lendo os registros do 'dados.bin'
    FILE *arquivo_dados = abrir_arquivo(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        perror("Arquivo de dados não encontrado. Não é possível recriar índice");
        return;
    }

    tamanho_vetor_indice_primario = 0;
    long offset;
    while (fread(&offset, sizeof(long), 1, arquivo_dados)) {
        char string_formatada[256];
        int tamanho_string;

        fread(&tamanho_string, sizeof(int), 1, arquivo_dados);
        fread(string_formatada, sizeof(char), tamanho_string, arquivo_dados);

        sscanf(string_formatada, "%4s#%4s", vetor_indice_primario[tamanho_vetor_indice_primario].id_Aluno, vetor_indice_primario[tamanho_vetor_indice_primario].sigla_Disciplina);

        vetor_indice_primario[tamanho_vetor_indice_primario].offset = offset;
        tamanho_vetor_indice_primario++;
    }

    fechar_arquivo(arquivo_dados);

    keysort();
    salvar_indice_primario(nome_arquivo_indice_primario);
}

void imprimir_registro_offset(const char *nome_arquivo_dados, long offset) {
    // Função para imprimir registro do 'dados.bin' dado um offset
    FILE *arquivo_dados = fopen(nome_arquivo_dados, "rb");
    if (arquivo_dados == NULL) {
        perror("Erro ao abrir arquivo de dados");
        return;
    }
    printf("offset %ld\n", offset);
    fseek(arquivo_dados, offset - 4, SEEK_SET);
    int tamanho_string;
    fread(&tamanho_string, sizeof(int), 1, arquivo_dados);
    printf("tamanho string: %d\n", tamanho_string);
    char string_formatada[128];
    fread(string_formatada, sizeof(char), tamanho_string, arquivo_dados);
    string_formatada[tamanho_string] = '\0' ;
    printf("Registro encontrado: %s\n", string_formatada);
    fclose(arquivo_dados);
}
