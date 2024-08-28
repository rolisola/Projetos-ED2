#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "arquivo.h"


// Funcao para verificar se o modo e valido
int validar_modo(const char *modo){
    // Modos permitidos
    const char *modos_validos[] = {"r", "rb", "w", "wb", "a", "ab", "r+", "rb+", "w+", "wb+", "a+", "ab+"};
    size_t num_modos_validos = sizeof(modos_validos) / sizeof(modos_validos[0]);

    // Verificar se o modo e um dos modos validos
    for(size_t i = 0; i < num_modos_validos; ++i){
        if(strcmp(modo, modos_validos[i]) == 0){
            return 1; // Modo valido
        }
    }
    return 0; // Modo invalido
}

// Funcao para abrir um arquivo
FILE *abrir_arquivo(const char *nome_arquivo, const char *modo){
    if(nome_arquivo == NULL || modo == NULL){
        fprintf(stderr, "Nome do arquivo ou modo e NULL.\n");
        return NULL;
    }

    if(!validar_modo(modo)){
        fprintf(stderr, "Modo invalido: (%s)\n", modo);
        return NULL;
    }

    return fopen(nome_arquivo, modo);
}

// Funcao para criar um arquivo
FILE *criar_arquivo(const char *nome_arquivo, const char *modo){
    if(nome_arquivo == NULL || modo == NULL){
        fprintf(stderr, "nome_arquivo or modo is NULL.\n");
        return NULL;
    }

    // Ajustar modo para criar o arquivo se necessario
    if(strchr(modo, 'r') != NULL){
        // Modificar o modo para 'w+' se o arquivo nao existir
        return fopen(nome_arquivo, "w+");
    }else{
        fprintf(stderr, "Nao pode criar arquivo com o modo: (%s)\n", modo);
        return NULL;
    }
}

// Funcao para abrir ou criar um arquivo
FILE *abrir_criar_arquivo(const char *nome_arquivo, const char *modo){
    FILE *arquivo = abrir_arquivo(nome_arquivo, modo);

    if(arquivo == NULL){
        if(errno == ENOENT){
            // Se o arquivo nao existe, cria o arquivo
            arquivo = criar_arquivo(nome_arquivo, modo);
        }else{
            // Se o erro nao for 'file not found', exibe mensagem de erro
            perror("Erro ao abrir arquivo");
        }
    }

    return arquivo;
}

// Funcao para fechar um arquivo
int fechar_arquivo(FILE *arquivo){
    if(arquivo == NULL){
        fprintf(stderr, "Ponteiro do arquivo invlido.\n");
        return 1; // Codigo de erro
    }

    if(fclose(arquivo) != 0){
        perror("Erro ao fechar arquivo.\n");
        return 1; // Codigo de erro
    }

    return 0; // Codigo de sucesso
}
