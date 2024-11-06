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

#define MAXKEYS 3

#define NIL -1
#define NOKEY '#'

#define PAGESIZE 38     // (1 int)+(3*2 char)+(3*1 int)+(4*1 int) = 4 + 6 + 12 + 16 = 38

typedef struct  {
    int regLidos_insere;
    int regLidos_busca;
}CABECALHO;

struct BTPage {
    int keycount;                       // Número de chaves na página
    char key[MAXKEYS][2];               // As chaves em si
    int BOF_arq_registros[MAXKEYS];     // BOFs dos registros no arquivo principal
    int child[MAXKEYS+1];               // RRNs dos filhos
};

bool existeArqRegistros() {
	if(access("arq_registros.bin", F_OK) == 0)
		return true;

	return false;
}

bool btopen(FILE **fp_BTree) {
    if ((*fp_BTree = fopen("BTree.bin", "r+b")) == NULL) {
        return false;
	}

    return true;
}

void btread(int RRN, struct BTPage *page_ptr, FILE *fp_BTree) {
    int addr;

    addr = RRN * PAGESIZE + sizeof(int);
    fseek(fp_BTree, addr, SEEK_SET);

    fread(&page_ptr->keycount, sizeof(int), 1, fp_BTree);

    fread(page_ptr->key[0], 2*sizeof(char), 1, fp_BTree);
    fread(page_ptr->key[1], 2*sizeof(char), 1, fp_BTree);
    fread(&page_ptr->key[2][0], sizeof(char), 1, fp_BTree);
    fread(&page_ptr->key[2][1], sizeof(char), 1, fp_BTree);

    fread(page_ptr->BOF_arq_registros, 3*sizeof(int), 1, fp_BTree);

    fread(&page_ptr->child, (MAXKEYS+1)*sizeof(int), 1, fp_BTree);
}

void btwrite(int RRN, struct BTPage *page_ptr, FILE *fp_BTree) {
    int addr;

    addr = RRN * PAGESIZE + sizeof(int);
    fseek(fp_BTree, addr, SEEK_SET);

    fwrite(&page_ptr->keycount, sizeof(int), 1, fp_BTree);

    fwrite(page_ptr->key[0], 2*sizeof(char), 1, fp_BTree);
    fwrite(page_ptr->key[1], 2*sizeof(char), 1, fp_BTree);
    fwrite(&page_ptr->key[2][0], sizeof(char), 1, fp_BTree);
    fwrite(&page_ptr->key[2][1], sizeof(char), 1, fp_BTree);

    fwrite(&page_ptr->BOF_arq_registros, 3*sizeof(int), 1, fp_BTree);

    fwrite(&page_ptr->child, (MAXKEYS+1)*sizeof(int), 1, fp_BTree);
}

int getroot(FILE *fp_BTree) {
    int root;

    fseek(fp_BTree, 0, SEEK_SET);
    if(!(fread(&root, sizeof(int), 1, fp_BTree))) {
        printf("Erro ao ler raiz");
        exit(1);
    }

    return root;
}

void putroot(int root, FILE *fp_BTree) {
    fseek(fp_BTree, 0, SEEK_SET);
    fwrite(&root, sizeof(int), 1, fp_BTree);
}

int getpageRRN(FILE *fp_BTree) {
    int addr;

    fseek(fp_BTree, 0, SEEK_END);
    addr = (ftell(fp_BTree) - sizeof(int)) / PAGESIZE;

    return addr;
}

void pageinit(struct BTPage *page_ptr) {
    int i;
    for(i=0; i<MAXKEYS; i++) {
        page_ptr->key[i][0] = NOKEY;
        page_ptr->key[i][1] = NOKEY;
        page_ptr->BOF_arq_registros[i] = NIL;
        page_ptr->child[i] = NIL;
    }
    page_ptr->child[i] = NIL;

    page_ptr->keycount = 0;
}

int create_empty_root(FILE *fp_BTree) {
    struct BTPage page;
    int RRN;

    RRN = getpageRRN(fp_BTree);
    pageinit(&page);
    btwrite(RRN, &page, fp_BTree);

    putroot(RRN, fp_BTree);
    return(RRN);
}

int create_root(FILE *fp_BTree, char *key, int BOF_arq_principal, int left, int right, struct BTPage *page_ptr) {
    int RRN;
    RRN = getpageRRN(fp_BTree);
    pageinit(page_ptr);
    page_ptr->keycount = 1;
    page_ptr->key[0][0] = key[0];
    page_ptr->key[0][1] = key[1];
    page_ptr->BOF_arq_registros[0] = BOF_arq_principal;
    page_ptr->child[0] = left;
    page_ptr->child[1] = right;

    btwrite(RRN, page_ptr, fp_BTree);
    putroot(RRN, fp_BTree);

    return(RRN);
}

int create_tree(FILE **fp_BTree) {
    int menosum = NIL;

    if ((*fp_BTree = fopen("BTree.bin", "w+b")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return 0;
	}

    fseek(*fp_BTree, 0, SEEK_SET);
    fwrite(&menosum, sizeof(int), 1, *fp_BTree);

    return(create_empty_root(*fp_BTree));
}

bool search_node(char *key, struct BTPage *page_ptr, int *pos) {
    int i;

    for(i=0; i < page_ptr->keycount; i++) {
        if(page_ptr->key[i][0] > key[0]) {
            *pos = i;
            return false;
        }

        else if(page_ptr->key[i][0] == key[0]) {
            if(page_ptr->key[i][1] == key[1]) {
                *pos = i;
                return true;
            }

            else if(page_ptr->key[i][1] > key[1]) {
                *pos = i;
                return false;
            }

            else if(page_ptr->key[i][1] < key[1]) {
            }
        }

        else if(page_ptr->key[i][0] < key[0]) {
        }
    }

    *pos = i;

    if(i < page_ptr->keycount && *pos < page_ptr->keycount && key[0] == page_ptr->key[*pos][0] && key[1] == page_ptr->key[*pos][1])
        return true;

    return false;
}

void ins_in_page(char *key, int r_child, int BOF_arq_principal, struct BTPage *page_ptr) {
    int i;

    for(i=page_ptr->keycount; i > 0 && key[0] <= page_ptr->key[i-1][0]; i--) {
        if(key[0] == page_ptr->key[i-1][0]) {
            if(key[1] > page_ptr->key[i-1][1]) break;
        }

        page_ptr->key[i][0] = page_ptr->key[i-1][0];
        page_ptr->key[i][1] = page_ptr->key[i-1][1];
        page_ptr->child[i+1] = page_ptr->child[i];
        page_ptr->BOF_arq_registros[i] = page_ptr->BOF_arq_registros[i-1];
    }

    page_ptr->keycount++;
    page_ptr->key[i][0] = key[0];
    page_ptr->key[i][1] = key[1];
    page_ptr->child[i+1] = r_child;
    page_ptr->BOF_arq_registros[i] = BOF_arq_principal;
}

void split(FILE *fp_BTree, char *key, int BOF_arq_principal, int r_child, struct BTPage *oldpage_ptr, char *promo_key, int *promo_r_child, int *promo_arq_BOF, struct BTPage *newpage_ptr) {
    char workkeys[MAXKEYS+1][2];
    int i, workchild[MAXKEYS+2], workBOFs[MAXKEYS+1];

    for(i=0; i < MAXKEYS; i++) {
        workkeys[i][0] = oldpage_ptr->key[i][0];
        workkeys[i][1] = oldpage_ptr->key[i][1];
        workchild[i] = oldpage_ptr->child[i];
        workBOFs[i] = oldpage_ptr->BOF_arq_registros[i];
    }
    workchild[i] = oldpage_ptr->child[i];

    for(i=MAXKEYS; i > 0 && key[0] <= workkeys[i-1][0]; i--) {
        if(key[0] == workkeys[i-1][0]) {
            if(key[1] > workkeys[i-1][1]) break;
        }

        workkeys[i][0] = workkeys[i-1][0];
        workkeys[i][1] = workkeys[i-1][1];
        workchild[i+1] = workchild[i];
        workBOFs[i] = workBOFs[i-1];
    }
    workkeys[i][0] = key[0];
    workkeys[i][1] = key[1];
    workchild[i+1] = r_child;
    workBOFs[i] = BOF_arq_principal;

    *promo_r_child = getpageRRN(fp_BTree);
    *promo_arq_BOF = workBOFs[1];
    pageinit(newpage_ptr);

    newpage_ptr->key[0][0] = workkeys[2][0];
    newpage_ptr->key[0][1] = workkeys[2][1];
    newpage_ptr->child[0] = workchild[2];
    newpage_ptr->BOF_arq_registros[0] = workBOFs[2];

    newpage_ptr->key[1][0] = workkeys[3][0];
    newpage_ptr->key[1][1] = workkeys[3][1];
    newpage_ptr->child[1] = workchild[3];
    newpage_ptr->child[2] = workchild[4];
    newpage_ptr->BOF_arq_registros[1] = workBOFs[3];

    newpage_ptr->keycount = 2;

    oldpage_ptr->key[1][0] = NOKEY;
    oldpage_ptr->key[1][1] = NOKEY;
    oldpage_ptr->BOF_arq_registros[1] = NIL;

    oldpage_ptr->key[2][0] = NOKEY;
    oldpage_ptr->key[2][1] = NOKEY;
    oldpage_ptr->child[2] = NIL;
    oldpage_ptr->BOF_arq_registros[2] = NIL;

    oldpage_ptr->keycount = 1;

    promo_key[0] = workkeys[1][0];
    promo_key[1] = workkeys[1][1];

    printf("\nDivisão de Nó\n");
    printf("Chave %c-%c promovida", promo_key[0], promo_key[1]);
}

bool inserir_arvore(FILE *fp_BTree, int RRN, char *key, int *promo_r_child, int *promo_arq_BOF, char *promo_key, bool *insert) {
    struct BTPage page, newpage;
    bool found, promoted;
    int pos, p_b_RRN;
    char p_b_key[2];

    if (RRN == NIL) {
        promo_key[0] = key[0];
        promo_key[1] = key[1];
        *promo_r_child = NIL;
        return true;
    }

    else {
        btread(RRN, &page, fp_BTree);
        found = search_node(key, &page, &pos);
    }

    if(found) {
        printf("\nChave %c-%c duplicada\n", key[0], key[1]);
        *insert = false;
        return false;
    }

    promoted = inserir_arvore(fp_BTree, page.child[pos], key, &p_b_RRN, promo_arq_BOF, p_b_key, insert);

    if(!promoted)
        return false;


    if(page.keycount < MAXKEYS) {
        ins_in_page(p_b_key, p_b_RRN, *promo_arq_BOF, &page);

        btwrite(RRN, &page, fp_BTree);
        return false;
    }

    else {
        split(fp_BTree, p_b_key, *promo_arq_BOF, p_b_RRN, &page, promo_key, promo_r_child, promo_arq_BOF, &newpage);
        btwrite(RRN, &page, fp_BTree);
        btwrite(*promo_r_child, &newpage, fp_BTree);
        return true;
    }
}

// Insere o registro no arquivo principal e chama a função para incluí-lo na Árvore B:
void inserir(FILE **fp_insere, FILE **fp_BTree, FILE *fp_arq) {

    // Abrindo o arquivo insere.bin para leitura:
    if ((*fp_insere = fopen("insere2.bin", "r+b")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
	}

    REGISTRO registro;
    CABECALHO header;

    // Lendo o header do arquivo principal para saber quantos registros já foram lidos:
    rewind(fp_arq);
    fread(&header, sizeof(CABECALHO), 1, fp_arq);

    // Acessando e lendo o registro correspondente do arquivo insere.bin:
    fseek(*fp_insere, header.regLidos_insere * sizeof(REGISTRO), SEEK_SET);
    fread(&registro, sizeof(REGISTRO), 1, *fp_insere);

    // Inserindo o registro no final do arquivo principal:
    fseek(fp_arq, 0, SEEK_END);
    int current_BOF = ftell(fp_arq);
    fwrite(&registro, sizeof(REGISTRO), 1, fp_arq);

    int root, promo_RRN, promo_arq_BOF = current_BOF;
    char key[2], promo_key[2];
    bool insert = true;

    key[0] = registro.id_Aluno[1];
    key[1] = registro.sigla_Disciplina[1];
    printf("\n%c-%c", key[0], key[1]);

    if(btopen(fp_BTree))
        root = getroot(*fp_BTree);

    else
        root = create_tree(fp_BTree);

    // Incluindo a chave bufferizada na Árvore:
    bool promoted = inserir_arvore(*fp_BTree, root, key, &promo_RRN, &promo_arq_BOF, promo_key, &insert);

    struct BTPage aux_page;

    if(promoted) {
        int promo_l_child = root;

        root = create_root(*fp_BTree, promo_key, promo_arq_BOF, promo_l_child, promo_RRN, &aux_page);
    }

    if(insert)
        printf("\nChave %c-%c incluída com sucesso!\n", key[0], key[1]);

    // Atualizando o contador de registros lidos do header do arquivo principal:
    header.regLidos_insere++;
    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);

    fclose(*fp_BTree);
    fclose(*fp_insere);
}


bool pesquisa(FILE *fp_BTree, int RRN, char *key, int *found_RRN, int *found_pos, int *arq_principal_BOF) {
    bool found;
    struct BTPage page;
    int pos;

    if(RRN == NIL)
        return false;

    else {
        btread(RRN, &page, fp_BTree);
        found = search_node(key, &page, &pos);

        if(found) {
            *found_RRN = RRN;
            *found_pos = pos;
            *arq_principal_BOF = page.BOF_arq_registros[pos];
            return true;
        }

        else return(pesquisa(fp_BTree, page.child[pos], key, found_RRN, found_pos, arq_principal_BOF));
    }
}

void buscar(FILE **fp_busca, FILE **fp_BTree, FILE *fp_arq) {
    // Abrindo o arquivo busca.bin para leitura:
    if ((*fp_busca = fopen("busca.bin", "r+b")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
	}

    char key_busca[6];
    CABECALHO header;

    // Lendo o header do arquivo principal para saber quantos registros já foram lidos:
    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    // Acessando e lendo o registro correspondente do arquivo busca.bin:
    fseek(*fp_busca, header.regLidos_busca * 6, SEEK_SET);
    fread(key_busca, 6, 1, *fp_busca);

    char key[2];
    int RRN, found_RRN, found_pos, arq_principal_BOF;
    REGISTRO registro;

    if(btopen(fp_BTree))
        RRN = getroot(*fp_BTree);

    else {
        printf("\nÁrvore vazia\n");
        return;
    }

    key[0] = key_busca[1];
    key[1] = key_busca[4];
    printf("\n%c-%c", key[0], key[1]);

    bool found = pesquisa(*fp_BTree, RRN, key, &found_RRN, &found_pos, &arq_principal_BOF);

    if(found) {
        printf("\nChave %c-%c encontrada, página %d, posição %d\n", key[0], key[1], found_RRN, found_pos);

        fseek(fp_arq, arq_principal_BOF, SEEK_SET);
        fread(&registro, sizeof(REGISTRO), 1, fp_arq);
        printf("ID Aluno: %s\n", registro.id_Aluno);
        printf("Sigla Disciplina: %s\n", registro.sigla_Disciplina);
        printf("Nome Aluno: %s\n", registro.nome_Aluno);
        printf("Nome Disciplina: %s\n", registro.nome_Disciplina);
        printf("Média: %.2f\n", registro.media);
        printf("Frequência: %.2f\n", registro.frequencia);

    }

    else
        printf("\nChave %c-%c não encontrada\n", key[0], key[1]);

    // Atualizando o contador de registros buscados no header do arquivo principal:
    header.regLidos_busca++;
    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);

    fclose(*fp_BTree);
    fclose(*fp_busca);
}

void percurso_emOrdem_BTree(int root_RRN, FILE *fp_BTree, FILE *fp_arq) {
    REGISTRO registro;
    struct BTPage page;
    int i;

    if(root_RRN == NIL) {
        return;
    }

    if(root_RRN != NIL) {
        btread(root_RRN, &page, fp_BTree);

        for(i=0; i<page.keycount+1; i++) {
            percurso_emOrdem_BTree(page.child[i], fp_BTree, fp_arq);
            if(i < page.keycount) {
                fseek(fp_arq, page.BOF_arq_registros[i], SEEK_SET);
                fread(&registro, sizeof(registro), 1, fp_arq);
                printf("ID Aluno: %s\n", registro.id_Aluno);
                printf("Sigla Disciplina: %s\n", registro.sigla_Disciplina);
                printf("Nome Aluno: %s\n", registro.nome_Aluno);
                printf("Nome Disciplina: %s\n", registro.nome_Disciplina);
                printf("Média: %.2f\n", registro.media);
                printf("Frequência: %.2f\n", registro.frequencia);
            }
        }
    }
}

void listar_todos_clientes(FILE **fp_BTree, FILE *fp_arq) {
    int root_RRN;

    if(btopen(fp_BTree))
        root_RRN = getroot(*fp_BTree);

    else {
        printf("\nÁrvore vazia\n");
        return;
    }

    percurso_emOrdem_BTree(root_RRN, *fp_BTree, fp_arq);

    fclose(*fp_BTree);
}

void printRegistro(REGISTRO reg) {
    printf("ID Aluno: %s\n", reg.id_Aluno);
    printf("Sigla Disciplina: %s\n", reg.sigla_Disciplina);
    printf("Nome Aluno: %s\n", reg.nome_Aluno);
    printf("Nome Disciplina: %s\n", reg.nome_Disciplina);
    printf("Media: %.2f\n", reg.media);
    printf("Frequencia: %.2f\n", reg.frequencia);
}

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
