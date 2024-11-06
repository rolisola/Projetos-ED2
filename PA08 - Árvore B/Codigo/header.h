#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

typedef size_t ENDERECO;
typedef float* NUMERO;
typedef int** CONJUNTO;
typedef char* STRING;
typedef char** PARAGRAFO;
typedef char*** TEXTO;

#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3

typedef struct dados{
    STRING id_Aluno[FIXO_ID + 1];
    STRING sigla_Disciplina[FIXO_SIGLA + 1];
    STRING nome_Aluno[MAX_NOME];
    STRING nome_Disciplina[MAX_NOME];
    NUMERO media;
    NUMERO frequencia;/*
    STRING codCliente;
    STRING codVeiculo;
    STRING nomeCliente;
    STRING nomeVeiculo;
    NUMERO quantDias;*/
} DADOS;

typedef DADOS* REGISTRO;
typedef DADOS** PASTA;
typedef DADOS*** GAVETA;


/*Adição nas Alocações Dinâmicas*/

void add_caractere_string(STRING* str, char caractere);
void add_string_string(STRING* str1, STRING* str2);
void add_string_paragrafo(PARAGRAFO* par, STRING* str);
void add_paragrafo_paragrafo(PARAGRAFO* par1, PARAGRAFO* par2);
void add_registro_pasta(PASTA* pst, REGISTRO* reg);
void add_pasta_pasta(PASTA* pst1, PASTA* pst2);


/*Remoção nas Alocações Dinâmicas*/

void rmv_first_caractere_string(STRING* str);
void rmv_first_string_paragrafo(PARAGRAFO* par);
void rmv_first_registro_pasta(PASTA* pst);

void rmv_last_caractere_string(STRING* str);
void rmv_last_string_paragrafo(PARAGRAFO* par);
void rmv_last_registro_pasta(PASTA* pst);


/*Copia nas Alocações Dinâmicas - FEITO*/
void copiar_string(STRING* new, STRING* str);

/*Tamanho nas Alocações Dinâmicas - FEITO*/
int tam_pasta(PASTA* pst);
int tam_paragrafo(PARAGRAFO* par);
size_t tam_registro(REGISTRO* reg);


/*Comparação nas Alocações Dinâmicas*/

int comparar_paragrafos(PARAGRAFO* par1, PARAGRAFO* par2);
int comparar_registros(REGISTRO* reg1, REGISTRO* reg2);
int comparar_pastas(PASTA* pst1, PASTA* pst2);

REGISTRO criar_registro();
NUMERO criar_numero();
STRING criar_string();

void limpar_numero(NUMERO* num);
void limpar_string(STRING* str);
void limpar_paragrafo(PARAGRAFO* par);
void limpar_registro(REGISTRO* reg);
void limpar_pasta(PASTA* pst);


void atualiza_log(char* sentenca) {

    bool inexistencia;
    FILE* log;
    
    log = fopen("log.txt", "r");
    
    if (log == NULL) {
        inexistencia = true;
    } else {
        inexistencia = false;
    }

    fclose(log);

    log = fopen("log.txt", "a+");
    if (inexistencia) {
        fprintf(log,"       REGISTRO DE ATIVIDADE:\n");
    }
    fprintf(log,"  # %s\n", sentenca);
    fclose(log);
}

void limpar_log() {
    remove("log.txt");
}