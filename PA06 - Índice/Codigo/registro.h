#ifndef REGISTRO_H
#define REGISTRO_H

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

typedef struct {
    char id[FIXO_ID + 1];
    char sigla[FIXO_SIGLA + 1];
    long offset;
} IndexEntry;

typedef struct {
    char nome[MAX_NOME];
    long offset;
} SecondaryIndexEntry;

typedef struct {
    char id_sigla[FIXO_ID + FIXO_SIGLA + 2]; // ID + Sigla + '\0'
    long offset;
} SecondaryListEntry;

// FUNÇÕES PRINCIPAIS //

void inserir_registro(const char *data_filename, const char *primary_index_filename, const char *secondary_index_filename, const REGISTRO *records, size_t num_records);
void buscar_por_chave_primaria(const char *data_filename, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria);

// FUNÇÕES AUXILIARES //

REGISTRO *carregar_insere(const char *nome_arquivo_insere);
size_t contar_registros(const char *nome_arquivo);
int obter_auxiliar(int posicao);
void atualizar_auxiliar(int posicao, int valor);
void imprime_vetor_insere(REGISTRO *vetor_insere, size_t tamanho_vetor_inserir);
void save_primary_index(const char *index_filename);
void save_secondary_index(const char *index_filename);
CHAVEPRIMARIA *carregar_busca_primaria(const char *nome_arquivo_busca_primaria);
void carregar_indice_primario(const char *primary_index_filename);
void imprime_vetor_chave_primaria(CHAVEPRIMARIA *vetor_chave_primaria, int tamanho_vetor_chave_primaria);
int compare_primary_index(const void *a, const void *b);
int comparador_chave_primaria(const void *a, const void *b);

void pesquisar_por_chave_primaria(const char *nome_arquivo_historico, const char *nome_arquivo_indice, const char *nome_arquivo_busca_primaria);
#endif
