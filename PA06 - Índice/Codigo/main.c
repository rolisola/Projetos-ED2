#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"
#include "registro.h"

int main() {
    remove("historico.bin");
    remove("auxiliar.bin");

    int opcao;
    const char *nome_arquivo_dados = "dados.bin";
    const char *nome_arquivo_indice = "indice.bin";
    const char *nome_arquivo_insere = "insere.bin";
    const char *nome_arquivo_busca_primaria = "busca_p.bin";
    const char *nome_arquivo_busca_secundaria = "busca_s.bin";
    const char *nome_arquivo_auxiliar = "auxiliar.bin";

    FILE *arquivo_auxiliar = abrir_criar_arquivo(nome_arquivo_auxiliar, "wb+");
    int valor = 0;
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    fwrite(&valor, sizeof(int), 1, arquivo_auxiliar);
    fclose(arquivo_auxiliar);

    REGISTRO *vr;
    vr = carregar_insere();
    size_t tamanho_vetor_inserir = contar_registros(nome_arquivo_insere);
    // a(vr);
    do {
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Busca Primária\n");
        printf("3. Busca Secundária\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            // inserir_registro(nome_arquivo_dados, vr, tamanho_vetor_inserir);
            inserir_registro(vr, nome_arquivo_dados);
            break;

        case 2:
            printf("Buscando.\n");
            //pesquisar_por_chave_primaria(nome_arquivo_dados, nome_arquivo_indice, nome_arquivo_busca_primaria);
            break;

        case 3:
            printf("\n");
            break;

        case 0:
            printf("Programa finalizado!\n");
            break;

        default:
            printf("Opção inválida, tente novamente!\n\n\n");
            break;
        }
    } while (opcao != 0);
    return 0;
}
