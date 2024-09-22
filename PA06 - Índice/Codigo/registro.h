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

/*typedef struct{
    int tamanho;
    int prox;
}EspacoDisponivel;*/

typedef struct {
    int posInsercao;
    int posRemocao;
} Estado;

REGISTRO *carregar_insere();
void inserir_registro(const char *nome_arquivo_historico, REGISTRO *vetor_insere, size_t tamanho_vetor_insere);
int obter_auxiliar(int posicao);
void atualizar_auxiliar(int posicao, int valor);
size_t contar_registros(const char *nome_arquivo);

// FUNÇÕES AUXILIARES

void imprime_vetor_insere(REGISTRO *vetor_insere);

// Funcoes relacionadas a arquivos e registros
// FILE* carregar_Historico(const char* nomeArquivo);
// void salvarEstado(Estado estado);
// Estado carregarEstado();
// void inserirRegistro(FILE *arquivo, Registro reg, Estado *estado);
// void removerRegistro(FILE *arquivo, char *idAluno, char *siglaDisciplina, Estado *estado);
// void compactarArquivo(const char* nomeArquivo);

#endif
