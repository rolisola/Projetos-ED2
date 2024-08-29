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
        printf("Nome do arquivo ou modo e NULL.\n");
        return NULL;
    }

    if(!validar_modo(modo)){
        printf("Modo invalido: (%s)\n", modo);
        return NULL;
    }

    return fopen(nome_arquivo, modo);
}

// Função para criar um arquivo
FILE *criar_arquivo(const char *nome_arquivo, const char *modo){
    if(nome_arquivo == NULL || modo == NULL){
        fprintf(stderr, "Nome do arquivo ou modo é NULL.\n");
        return NULL;
    }

    // Ajustar modo para criar o arquivo se necessário
    if(strchr(modo, 'r') != NULL){
        // Verifica se o modo e binário
        if(strchr(modo, 'b') != NULL){
            // Se o modo e binário, usar "wb+"
            return fopen(nome_arquivo, "wb+");
        }else{
            // Caso contrário, usar "w+"
            return fopen(nome_arquivo, "w+");
        }
    }else{
        fprintf(stderr, "Nao pode criar arquivo com o modo: (%s)\n", modo);
        return NULL;
    }
}

// Função para abrir ou criar um arquivo
FILE *abrir_criar_arquivo(const char *nome_arquivo, const char *modo){
    FILE *arquivo = abrir_arquivo(nome_arquivo, modo);

    if(arquivo == NULL){
        if(errno == ENOENT){
            // Se o arquivo não existe, cria o arquivo
            printf("Arquivo inexistente. Criando.\n");
            arquivo = criar_arquivo(nome_arquivo, modo);
        }else{
            // Se o erro não for 'file not found', exibe mensagem de erro
            perror("Erro ao abrir arquivo");
        }
    }

    return arquivo;
}

// Função para fechar um arquivo
int fechar_arquivo(FILE *arquivo){
    if(arquivo == NULL){
        fprintf(stderr, "Ponteiro do arquivo invlido.\n");
        return 1; // Código de erro
    }

    if(fclose(arquivo) != 0){
        perror("Erro ao fechar arquivo.\n");
        return 1; // Código de erro
    }

    return 0; // Código de sucesso
}

size_t obter_tamanho_arquivo(FILE *arquivo){
    fseek(arquivo, 0, SEEK_END); // Move o ponteiro para o final do arquivo
    size_t tamanho = ftell(arquivo); // Pega a posição atual do ponteiro (tamanho do arquivo)
    fseek(arquivo, 0, SEEK_SET); // Volta o ponteiro para o início do arquivo

    return tamanho;
}
