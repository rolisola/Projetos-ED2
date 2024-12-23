#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define VAZIO_VERDADEIRO -1
#define VAZIO_FALSO -2

#define PAGESIZE 8 // 2 int

#define N 13
#define MAX_NOME 50
#define FIXO_ID 3
#define FIXO_SIGLA 3

typedef struct {
    char id_aluno[FIXO_ID + 1];
    char sigla_disc[FIXO_SIGLA + 1];
    char nome_aluno[MAX_NOME];
    char nome_disc[MAX_NOME];
    float media;
    float freq;
} REGISTRO;

typedef struct {
    int regLidos_insere;
    int regLidos_busca;
    int regLidos_remove;
} CABECALHO;

typedef struct {
    int key;
    int BOF_arq_registros;
} HASH_PAGE;

bool existeArqRegistros() {
    if (access("arq_registros.bin", F_OK) == 0) {
        return true;
    }
    return false;
}

void pageinit(HASH_PAGE *page_ptr) {
    page_ptr->key = VAZIO_VERDADEIRO;
    page_ptr->BOF_arq_registros = -1;
}

void create_hash(FILE **fp_hash) {
    if ((*fp_hash = fopen("Hash_Table.bin", "wb+")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
    }

    HASH_PAGE page;
    pageinit(&page);

    int i;

    for (i = 0; i < N; i++) {
        fwrite(&page, sizeof(page), 1, *fp_hash);
    }
}

int hash(int key) {
    return (key % N) + 1;
}

size_t obter_tamanho_arquivo(FILE *arquivo) {
    fseek(arquivo, 0, SEEK_END);     // Move o ponteiro para o final do arquivo
    size_t tamanho = ftell(arquivo); // Pega a posição atual do ponteiro (tamanho do arquivo)
    fseek(arquivo, 0, SEEK_SET);     // Volta o ponteiro para o início do arquivo

    return tamanho;
}

int buscar_hash(char *id_aluno, char *sigla_disc, FILE *fp_hash, int *acessos, HASH_PAGE *page_ptr) {
    HASH_PAGE workpage;
    int addr, i, contador_acessos = 1;
    char key_string[8];

    strcpy(key_string, id_aluno);
    strcat(key_string, sigla_disc);
    page_ptr->key = atoi(key_string);

    addr = hash(page_ptr->key);

    for (i = addr - 1; contador_acessos <= N; i++) {
        if (i >= 13) {
            i -= 13;
        }
        if (i == -1) {
            i++;
        }
        fseek(fp_hash, i * PAGESIZE, SEEK_SET);
        fread(&workpage, sizeof(workpage), 1, fp_hash);

        if (page_ptr->key == workpage.key) {
            page_ptr = &workpage;
            *acessos = contador_acessos;
            return i + 1;
        }

        else if (workpage.key == VAZIO_VERDADEIRO) {
            break;
        }
        contador_acessos++;
    }

    return -1;
}

void buscar(FILE **fp_busca, FILE *fp_hash, FILE *fp_arq) {
    // Abrindo o arquivo busca.bin para leitura:
    if ((*fp_busca = fopen("busca.bin", "rb")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
    }

    HASH_PAGE page;
    CABECALHO header;
    char id_aluno[4], sigla_disc[4];

    size_t tamanho_arquivo = obter_tamanho_arquivo(*fp_busca);
    size_t quantidade_registros = tamanho_arquivo / (sizeof(id_aluno) + sizeof(sigla_disc));

    // Lendo o header do arquivo principal para saber quantos registros já foram lidos:
    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    if (header.regLidos_busca >= quantidade_registros) {
        printf("Todas as buscas foram realizadas.\n");
        return;
    }

    // Acessando e lendo o registro correspondente do arquivo busca.bin:
    fseek(*fp_busca, header.regLidos_busca * (8 * sizeof(char)), SEEK_SET);
    fread(id_aluno, sizeof(char), 4, *fp_busca);
    fread(sigla_disc, sizeof(char), 4, *fp_busca);

    REGISTRO registro;
    int acessos;
    int key_address = buscar_hash(id_aluno, sigla_disc, fp_hash, &acessos, &page);

    if (key_address != -1) {
        printf("\n'%s'+'%s'\nChave encontrada, endereço %d, %d acesso(s)\n", id_aluno, sigla_disc, key_address, acessos);
        fseek(fp_arq, page.BOF_arq_registros, SEEK_SET);
        fread(&registro, sizeof(registro), 1, fp_arq);
    }

    else {
        printf("\n'%s'+'%s'\nChave não encontrada\n", id_aluno, sigla_disc);
    }
    // Atualizando o contador de registros lidos do header do arquivo principal:
    header.regLidos_busca++;
    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);

    fclose(*fp_busca);
}

void remover(FILE **fp_remove, FILE *fp_hash, FILE *fp_arq) {
    // Abrindo o arquivo remove.bin para leitura:
    if ((*fp_remove = fopen("remove.bin", "rb")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
    }

    HASH_PAGE page;
    CABECALHO header;
    char id_aluno[4], sigla_disc[4];

    size_t tamanho_arquivo = obter_tamanho_arquivo(*fp_remove);
    size_t quantidade_registros = tamanho_arquivo / (sizeof(id_aluno) + sizeof(sigla_disc));

    // Lendo o header do arquivo principal para saber quantos registros já foram lidos:
    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    if (header.regLidos_remove >= quantidade_registros) {
        printf("Todas as remoções foram realizadas.\n");
        return;
    }

    // Acessando e lendo o registro correspondente do arquivo remove.bin:
    fseek(*fp_remove, header.regLidos_remove * (8 * sizeof(char)), SEEK_SET);
    fread(id_aluno, sizeof(char), 4, *fp_remove);
    fread(sigla_disc, sizeof(char), 4, *fp_remove);

    int acessos, NO_KEY = VAZIO_FALSO, NO_BOF = -1;
    int key_address = buscar_hash(id_aluno, sigla_disc, fp_hash, &acessos, &page);

    if (key_address == -1) {
        printf("\n'%s'+'%s'\nA chave não existe na tabela hash\n", id_aluno, sigla_disc);

    } else {
        fseek(fp_hash, (key_address - 1) * PAGESIZE, SEEK_SET);
        fwrite(&NO_KEY, sizeof(int), 1, fp_hash);
        fwrite(&NO_BOF, sizeof(int), 1, fp_hash);
        printf("\n'%s'+'%s'\nChave removida com sucesso\n", id_aluno, sigla_disc);
    }

    // Atualizando o contador de registros lidos do header do arquivo principal:
    header.regLidos_remove++;
    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);

    fclose(*fp_remove);
}

int inserir_hash(int key, int BOF_arq_registros, FILE *fp_hash, int *tentativas) {
    HASH_PAGE read_page, write_page;
    int addr, i, contador_tentativas = 0;

    write_page.key = key;
    write_page.BOF_arq_registros = BOF_arq_registros;

    addr = hash(key);

    for (i = addr - 1; contador_tentativas < N; i++) {
        if (i >= N) {
            i -= N;
        }
        if (i == -1) {
            i++;
        }
        fseek(fp_hash, i * PAGESIZE, SEEK_SET);
        fread(&read_page, sizeof(read_page), 1, fp_hash);

        if (read_page.key == VAZIO_VERDADEIRO || read_page.key == VAZIO_FALSO) {
            fseek(fp_hash, i * PAGESIZE, SEEK_SET);
            fwrite(&write_page, sizeof(write_page), 1, fp_hash);

            *tentativas = contador_tentativas;
            return i; // Return the address (1-based index)
        }

        contador_tentativas++;
    }

    return -1;
}

// Insere o registro no arquivo principal e chama a função para incluí-lo na árvore B:
void inserir(FILE **fp_insere, FILE *fp_hash, FILE *fp_arq) {
    if ((*fp_insere = fopen("insere.bin", "rb")) == NULL) {
        printf("Não foi possível abrir o arquivo");
        return;
    }

    HASH_PAGE page;
    REGISTRO registro;
    CABECALHO header;

    size_t tamanho_arquivo = obter_tamanho_arquivo(*fp_insere);
    size_t quantidade_registros = tamanho_arquivo / sizeof(registro);

    // Lendo o header do arquivo principal para saber quantos registros já foram lidos:
    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    if (header.regLidos_insere >= quantidade_registros) {
        printf("Todos os registros foram inseridos.\n");
        return;
    }

    // Acessando e lendo o registro correspondente do arquivo insere.bin:
    fseek(*fp_insere, header.regLidos_insere * sizeof(registro), SEEK_SET);
    fread(&registro, sizeof(registro), 1, *fp_insere);

    int acessos, tentativas = 0;
    int key_address = buscar_hash(registro.id_aluno, registro.sigla_disc, fp_hash, &acessos, &page);

    if (key_address != -1) {
        printf("\n'%s'+'%s'\nTentativa de inserção de chave repetida\n", registro.id_aluno, registro.sigla_disc);
    } else {
        // Inserindo o registro no final do arquivo principal:
        char buffer[256];
        sprintf(buffer, "%s#%s#%s#%s#%0.1f#%0.1f", registro.id_aluno, registro.sigla_disc, registro.nome_aluno, registro.nome_disc, registro.media, registro.freq);
        int tam_reg = strlen(buffer);

        fseek(fp_arq, 0, SEEK_END);
        int current_BOF = ftell(fp_arq);
        fwrite(&tam_reg, sizeof(int), 1, fp_arq);
        fwrite(buffer, tam_reg, 1, fp_arq);

        key_address = inserir_hash(page.key, current_BOF, fp_hash, &tentativas);

        if (key_address != -1) {
            printf("\n'%s'+'%s'\n", registro.id_aluno, registro.sigla_disc);
            printf("Endereço %d\n", key_address);

            if (tentativas > 0) {
                printf("Colisão\nTentativa %d\n", tentativas);
            }
            printf("Chave inserida com sucesso\n");
        } else {
            printf("\nTabela hash cheia\n");
        }
    }

    header.regLidos_insere++;
    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);
    rewind(fp_arq);

    fclose(*fp_insere);
}

int main() {
    FILE *fp_arq, *fp_insere, *fp_busca, *fp_remove, *fp_hash;

    CABECALHO header;

    // Se o arquivo principal não existe, abrir arq_registros.bin escrita:
    if (!existeArqRegistros()) {
        if ((fp_arq = fopen("arq_registros.bin", "wb+")) == NULL) {
            printf("Não foi possível abrir o arquivo");
            return 0;
        }

        // Inicializando o header do arquivo principal:
        header.regLidos_insere = 0;
        header.regLidos_busca = 0;
        header.regLidos_remove = 0;
        fwrite(&header, sizeof(header), 1, fp_arq);
        rewind(fp_arq);

        create_hash(&fp_hash);
    }

    // Caso contrário, abrir arq_registros.bin para leitura:
    else {
        if ((fp_arq = fopen("arq_registros.bin", "rb+")) == NULL) {
            printf("Não foi possível abrir o arquivo");
            return 0;
        }

        if ((fp_hash = fopen("Hash_Table.bin", "rb+")) == NULL) {
            printf("Não foi possível abrir o arquivo");
            return 0;
        }
    }

    // Menu:
    int opcao = 0;
    do {
        printf("Menu:\n");
        printf("1. Inserir\n");
        printf("2. Remover\n");
        printf("3. Buscar\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        switch (opcao) {
        case 1:
            printf("Inserindo.\n");
            inserir(&fp_insere, fp_hash, fp_arq);
            break;
        case 2:
            printf("Removendo.\n");
            remover(&fp_remove, fp_hash, fp_arq);
            break;
        case 3:
            printf("Buscando.\n");
            buscar(&fp_busca, fp_hash, fp_arq);
            break;
        case 0:
            printf("Saindo.\n");
            fclose(fp_arq);
            fclose(fp_hash);
            break;
        default:
            printf("Opção inválida, tente novamente!\n\n\n");
            break;
        }
    } while (opcao != 0);

    return 0;
}
