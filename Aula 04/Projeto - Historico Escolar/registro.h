#ifndef REGISTRO_H
#define REGISTRO_H

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3

typedef struct {
    char id_Aluno[FIXO_ID+1];
    char sigla_Disciplina[FIXO_SIGLA+1];
    char nome_Aluno[MAX_NOME+1];
    char nome_Disciplina[MAX_NOME+1];
    float media;
    float frequencia;
} Registro;

typedef struct {
    int tamanho;
    int prox;
} EspacoDisponivel;

typedef struct {
    int posInsercao;
    int posRemocao;
} Estado;

// Funções relacionadas a arquivos e registros
FILE* carregar_Historico(const char* nomeArquivo);
void fechar_arquivo(FILE *arquivo);
void salvarEstado(Estado estado);
Estado carregarEstado();
void inserirRegistro(FILE *arquivo, Registro reg, Estado *estado);
void removerRegistro(FILE *arquivo, char *idAluno, char *siglaDisciplina, Estado *estado);
void compactarArquivo(const char* nomeArquivo);

#endif
