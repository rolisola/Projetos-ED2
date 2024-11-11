#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<assert.h>

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
    int regLidos_insere;
    int regLidos_busca;
} CABECALHO;

int main () {
    int opcao;
    FILE *fp_arq, *fp_insere, *fp_busca, *fp_BTree;

    CABECALHO header;

    // Se o arquivo principal não existe, abrir arq_registros.bin escrita:
    if(!existeArqRegistros()) {
        if ((fp_arq = fopen("arq_registros.bin","w+b")) == NULL) {
	    	printf("Não foi possível abrir o arquivo");
		    return 0;
	    }

        // Inicializando o header do arquivo principal:
        header.regLidos_insere = 0;
        header.regLidos_busca = 0;
        fwrite(&header, sizeof(header), 1, fp_arq);
        rewind(fp_arq);
    }

    // Caso contrário, abrir arq_registros.bin para leitura:
    else {
        if((fp_arq = fopen("arq_registros.bin","r+b")) == NULL) {
	    	printf("Não foi possível abrir o arquivo");
		    return 0;
	    }
    }

    // Menu:
    do {
		printf("Menu:\n");
		printf("1. Inserir\n");
		printf("2. Listar todos os dados\n");
		printf("3. Listar  dados de um aluno numa disciplina\n");
		printf("4. Sair\n");
		printf("Opção: ");
		scanf(" %d", &opcao);
        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            inserir(&fp_insere, &fp_BTree, fp_arq);
            break;

        case 2:
            printf("Buscando.\n");
            listar_todos_clientes(&fp_BTree, fp_arq);
            break;

        case 3:
            printf("Buscando.\n");
            buscar(&fp_busca, &fp_BTree, fp_arq);
            break;
        case 4:
            //salvar_indice_primario(nome_arquivo_indice_primario);
            break;
        case 5:
            //  imprime_vetor_indice_primario();
            break;
        case 0:
            fclose(fp_arq);
            printf("Salvando e saindo.\n");
            printf("Programa finalizado!\n");
            break;

        default:
            printf("Opção inválida, tente novamente!\n\n\n");
            break;
        }
    } while (!((opcao == 0) || (opcao == 4)));
}
