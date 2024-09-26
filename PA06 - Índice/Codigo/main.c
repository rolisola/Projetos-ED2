#include <stdio.h>
#include <stdlib.h>
#include "registro.h"

int main() {
    int opcao;
    const char *nome_arquivo_dados = "dados.bin";
    const char *nome_arquivo_insere = "insere.bin";
    const char *nome_arquivo_busca_primaria = "busca_p.bin";
    const char *primary_index_filename = "indice_primario.bin";
    const char *nome_arquivo_busca_secundaria = "busca_s.bin";
    const char *secondary_index_filename = "indice_secundario.bin";

    REGISTRO *vetor_registros;
    vetor_registros = carregar_insere(nome_arquivo_insere);
    size_t tamanho_vetor_inserir = contar_registros(nome_arquivo_insere);
    //imprime_vetor_insere(vetor_registros, tamanho_vetor_inserir);

    CHAVEPRIMARIA *vetor_chave_primaria;
    vetor_chave_primaria = carregar_busca_primaria(nome_arquivo_busca_primaria);
    size_t tamanho_vetor_busca_primaria = contar_registros(nome_arquivo_busca_primaria);
    //imprime_vetor_chave_primaria(vetor_chave_primaria, tamanho_vetor_busca_primaria);

    do {
        printf("Menu:\n");
        printf("1. Inserção\n");
        printf("2. Busca Primária\n");
        printf("3. Busca Secundária\n");
        //printf("4. Sair e salvar\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            inserir_registro(nome_arquivo_dados, primary_index_filename, secondary_index_filename, vetor_registros, tamanho_vetor_inserir);
            break;

        case 2:
            printf("Buscando.\n");
            buscar_por_chave_primaria(nome_arquivo_dados, vetor_chave_primaria, tamanho_vetor_busca_primaria);
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
