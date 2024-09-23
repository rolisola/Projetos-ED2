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
} IndicePrimario;

typedef struct {
    char id_Aluno[FIXO_ID + 1];
    char sigla_Disciplina[FIXO_SIGLA + 1];
} ChavePrimaria;

REGISTRO *carregar_insere();
// void inserir_registro(const char *nome_arquivo_historico, REGISTRO *vetor_insere, size_t tamanho_vetor_insere);
void inserir_registro(REGISTRO *vetor_insere, const char *nome_arquivo_dados);
int obter_auxiliar(int posicao);
void atualizar_auxiliar(int posicao, int valor);
size_t contar_registros(const char *nome_arquivo);
int comparador_chave_primaria(const void *a, const void *b);
void pesquisar_por_chave_primaria(const char *nome_arquivo_historico, const char *nome_arquivo_indice, const char *nome_arquivo_busca_primaria);

// FUNÇÕES AUXILIARES
void imprime_vetor_insere(REGISTRO *vetor_insere);

#endif
