#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "arquivo.h"

IndexEntry *primary_index = NULL;
SecondaryIndexEntry *secondary_index = NULL;
SecondaryListEntry *secondary_list = NULL;
int primary_index_size = 0;
int secondary_index_size = 0;
int secondary_list_size = 0;

// FUNÇÕES PRINCIPAIS //

void inserir_registro(const char *data_filename, const char *primary_index_filename, const char *secondary_index_filename, const REGISTRO *vetor_registro, size_t tamanho_vetor_insere) {
    // Insere o registro do vetor no historico.bin
    int posicao = obter_auxiliar(0);
    if (posicao >= tamanho_vetor_insere) {
        printf("Todos os registros foram inseridos!\n");
        return;
    }
    printf("Inserindo registro: %d\n", posicao + 1);

    FILE *data_file = abrir_criar_arquivo(data_filename, "ab");
    if (!data_file) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }

    // Converter a estrutura REGISTRO em uma string no formato especificado
    char buffer[256]; // Assumindo que o tamanho máximo do registro será menor que 256 bytes
    int record_size = snprintf(buffer, sizeof(buffer), "%s#%s#%s#%s#%.1f#%.1f",
                               vetor_registro[posicao].id_Aluno, vetor_registro[posicao].sigla_Disciplina, vetor_registro[posicao].nome_Aluno, vetor_registro[posicao].nome_Disciplina,
                               vetor_registro[posicao].media, vetor_registro[posicao].frequencia);

    // Escrever o registro no arquivo de dados
    fwrite(&record_size, sizeof(int), 1, data_file);
    long offset = ftell(data_file);
    printf("Offset %d\n", offset);
    fwrite(buffer, sizeof(char), record_size, data_file);

    // Atualizar índice primário
    primary_index = realloc(primary_index, (primary_index_size + 1) * sizeof(IndexEntry));
    snprintf(primary_index[primary_index_size].id, FIXO_ID + 1, "%s", vetor_registro[posicao].id_Aluno);
    //printf("Index primario idAluno %s\n", primary_index[primary_index_size].id);
    snprintf(primary_index[primary_index_size].sigla, FIXO_SIGLA + 1, "%s", vetor_registro[posicao].sigla_Disciplina);
    //printf("Index primario sigladisc %s\n", primary_index[primary_index_size].sigla);
    primary_index[primary_index_size].offset = offset;// - sizeof(int) - record_size;  <---- pq colocou isso aqui????
    primary_index_size++;
    //printf("Primary_index_size %d\n", primary_index_size);
    

    // Atualizar índice secundário
    int nome_index = -1;
    for (int j = 0; j < secondary_index_size; j++) {
        if (strcmp(secondary_index[j].nome, vetor_registro[posicao].nome_Aluno) == 0) {
            nome_index = j;
            break;
        }
    }

    if (nome_index == -1) {
        secondary_index = realloc(secondary_index, (secondary_index_size + 1) * sizeof(SecondaryIndexEntry));
        snprintf(secondary_index[secondary_index_size].nome, MAX_NOME, "%s", vetor_registro[posicao].nome_Aluno);
        secondary_index[secondary_index_size].offset = secondary_list_size * sizeof(SecondaryListEntry);
        nome_index = secondary_index_size;
        secondary_index_size++;
    }

    secondary_list = realloc(secondary_list, (secondary_list_size + 1) * sizeof(SecondaryListEntry));
    snprintf(secondary_list[secondary_list_size].id_sigla, FIXO_ID + FIXO_SIGLA + 2, "%s%s", vetor_registro[posicao].id_Aluno, vetor_registro[posicao].sigla_Disciplina);
    secondary_list[secondary_list_size].offset = -1;
    if (secondary_index[nome_index].offset != secondary_list_size * sizeof(SecondaryListEntry)) {
        int last_entry_index = secondary_index[nome_index].offset / sizeof(SecondaryListEntry);
        while (secondary_list[last_entry_index].offset != -1) {
            last_entry_index = secondary_list[last_entry_index].offset / sizeof(SecondaryListEntry);
        }
        secondary_list[last_entry_index].offset = secondary_list_size * sizeof(SecondaryListEntry);
    }

    secondary_list_size++;

    fclose(data_file);

    posicao++;
    atualizar_auxiliar(0, posicao);

    qsort(primary_index, primary_index_size, sizeof(IndexEntry), compare_primary_index);
    save_primary_index(primary_index_filename);
    save_secondary_index(secondary_index_filename);
}

void buscar_por_chave_primaria(const char *data_filename, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria) {
    int posicao = obter_auxiliar(1);
    if (posicao >= tamanho_vetor_busca_primaria) {
        printf("Todas as buscas por chave primaria ja foram realizadas!\n");
        return;
    }
    
    posicao++;
    printf("Buscando chave: %d\n", posicao);
    atualizar_auxiliar(1, posicao);

    char bufferkey[18];
    IndexEntry key;
    printf("%s\n", key[posicao].id);
    //snprintf(buffer, sizeof(buffer), "%s", key);
    //snprintf(key.sigla, FIXO_SIGLA + 1, "%s", key); // Assume que a chave é concatenada: ID+Sigla
    snprintf(bufferkey, sizeof(bufferkey), "%s %s ", key.id, key.sigla);
    printf("%s\n", key);

    IndexEntry *result = bsearch(&key, primary_index, primary_index_size, sizeof(IndexEntry), compare_primary_index);

    if (result) {
        FILE *data_file = fopen(data_filename, "rb");
        if (!data_file) {
            perror("Erro ao abrir o arquivo de dados");
            return;
        }

        fseek(data_file, result->offset, SEEK_SET);
        int record_size;
        fread(&record_size, sizeof(int), 1, data_file);

        char buffer[record_size + 1];
        fread(buffer, sizeof(char), record_size, data_file);
        buffer[record_size] = '\0';

        printf("Registro encontrado: %s\n", buffer);
        fclose(data_file);
     } else {
        printf("Registro não encontrado\n");
     }
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
    // Possiveis valores de entrada: 0 (insere.bin), 1 (busca_p.bin), 2 (busca_s.bin)
    FILE *arquivo_auxiliar = abrir_criar_arquivo("auxiliar.bin", "rb");
    if (arquivo_auxiliar == NULL) {
        perror("Erro ao abrir arquivo. SAINDO!!!\n");
        exit(EXIT_FAILURE);
    }

    int valor = 1;

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

void save_primary_index(const char *index_filename) {
    FILE *index_file = fopen(index_filename, "wb");
    if (!index_file) {
        perror("Erro ao abrir o arquivo de índice primário");
        return;
    }

    fwrite(primary_index, sizeof(IndexEntry), primary_index_size, index_file);
    fclose(index_file);
}

void save_secondary_index(const char *index_filename) {
    FILE *index_file = fopen(index_filename, "wb");
    if (!index_file) {
        perror("Erro ao abrir o arquivo de índice secundário");
        return;
    }

    fwrite(secondary_index, sizeof(SecondaryIndexEntry), secondary_index_size, index_file);
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

void carregar_indice_primario(const char *primary_index_filename) {
    FILE *index_file = fopen(primary_index_filename, "rb");
    if (!index_file) {
        perror("Erro ao abrir o arquivo de índice primário");
        return;
    }

    fseek(index_file, 0, SEEK_END);
    long file_size = ftell(index_file);
    fseek(index_file, 0, SEEK_SET);

    primary_index = malloc(file_size);
    if (!primary_index) {
        perror("Erro ao alocar memória para o índice primário");
        fclose(index_file);
        return;
    }

    fread(primary_index, file_size, 1, index_file);
    primary_index_size = file_size / sizeof(IndexEntry);

    fclose(index_file);
}

void imprime_vetor_chave_primaria(CHAVEPRIMARIA *vetor_chave_primaria, int tamanho_vetor_chave_primaria) {
    for (int i = 0; i < tamanho_vetor_chave_primaria; i++) {
        printf("%d\n", i);
        printf("Id Aluno:         (%s)\n", vetor_chave_primaria[i].id_Aluno);
        printf("Sigla Disciplina: (%s)\n\n", vetor_chave_primaria[i].sigla_Disciplina);
    }
}

int compare_primary_index(const void *a, const void *b) {
    return strcmp(((IndexEntry *)a)->id, ((IndexEntry *)b)->id);
}

int comparador_chave_primaria(const void *a, const void *b) {
    // Função de comparação para ordenar e buscar usando keysorting
    INDICEPRIMARIO *ia = (INDICEPRIMARIO *)a;
    INDICEPRIMARIO *ib = (INDICEPRIMARIO *)b;

    int resultado_id = strcmp(ia->id_Aluno, ib->id_Aluno);
    if (resultado_id != 0)
        return resultado_id;

    return strcmp(ia->sigla_Disciplina, ib->sigla_Disciplina);
}
