#ifndef REGISTRO_H
#define REGISTRO_H

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
/*
typedef struct {
    char nome[MAX_NOME];
    long offset;
} SecondaryIndexEntry;

typedef struct {
    char id_sigla[FIXO_ID + FIXO_SIGLA + 2]; // ID + Sigla + '\0'
    long offset;
} SecondaryListEntry;
*/

// FUNÇÕES PRINCIPAIS //

void inserir_registro(const char *nome_arquivo_dados, const char *nome_arquivo_indice_primario, const char *secondary_index_filename, const REGISTRO *vetor_insere, size_t tamanho_vetor_insere);
void buscar_por_chave_primaria(const char *nome_arquivo_dados, CHAVEPRIMARIA *vetor_busca_primaria, size_t tamanho_vetor_busca_primaria);

// FUNÇÕES AUXILIARES //

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
#endif

//void save_secondary_index(const char *index_filename);
