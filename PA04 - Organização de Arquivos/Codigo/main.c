#include <stdio.h>
#include "registro.h"


int main() {
    int opcao;
    const char *arquivo_Historico = "historico.bin";
    const char *arquivo_insere = "insere.bin";
    const char *arquivo_remove = "remove.bin";
    const char *arquivo_auxiliar = "aux.bin";

    //FILE *arquivo = carregar_Historico(arquivo_Historico);
    carregar_insere();

    do{
        // Exibe o menu
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Remoção\n");
        printf("3. Compactação\n");
        printf("4. Carregar Arquivo\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch(opcao){
            case 1:
                printf("Inserindo.\n");
                //escrever(arquivo_insere);
                break;
            case 2:
                printf("");
                break;
            case 3:
                printf("");
                break;
            case 4:
                printf("");
                break;
            case 0:
                printf("Programa finalizado!\n");
                break;
            default:
                printf("Opção inválida, tente novamente.\n\n\n");
                break;
        }
    }while(opcao != 0);



/*
    Estado estado = carregarEstado();

    // Exemplo de inserção
    Registro reg1 = {"001", "ED2", "Paulo da Silva", "Estrutura de Dados 2", 7.3, 75.4};
    inserirRegistro(arquivo, reg1, &estado);

    // Exemplo de remoção
    removerRegistro(arquivo, "001", "ED2", &estado);

    // Exemplo de compactação
    compactarArquivo(nome_Arquivo);

    fecharArquivo(arquivo);*/
    return 0;
}
