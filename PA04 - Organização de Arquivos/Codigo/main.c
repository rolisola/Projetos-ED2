#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

struct Reg {
    char idAluno[4];
    char siglaDisc[4];
    char nomeAluno[50];
    char nomeDisc[50];
    float media;
    float freq;
};

struct Chave {
    char idAluno[4];
    char siglaDisc[4];
};

struct Cabecalho {
    int regLidos_insere;
    int regLidos_remove;
    int BOF_dispo; // BOF=ByteOffSet
};

bool existeArq() {
    if (access("arq_registros.bin", F_OK) == 0)
        return true;

    return false;
}

size_t obter_tamanho_arquivo(FILE *arquivo) {
    fseek(arquivo, 0, SEEK_END);     // Move o ponteiro para o final do arquivo
    size_t tamanho = ftell(arquivo); // Pega a posição atual do ponteiro (tamanho do arquivo)
    fseek(arquivo, 0, SEEK_SET);     // Volta o ponteiro para o início do arquivo

    return tamanho;
}

int pega_registro(FILE *fp, char *buffer) {
    int pipeline_counter = 0;
    int tam_registro = 0;
    int i = 0;

    while (pipeline_counter < 5) {
        buffer[i] = fgetc(fp);
        if (buffer[i] == '#')
            pipeline_counter++;
        tam_registro++;
        i++;
    }

    buffer[i] = '\0';
    return tam_registro;
}

void inserir(FILE **fp_insere, FILE *fp_arq) {
    if ((*fp_insere = fopen("insere.bin", "r+b")) == NULL) {
        printf("Nao foi possivel abrir o arquivo (void inserir)");
        return;
    }

    struct Reg registro;
    struct Cabecalho header;

    size_t tamanho_arquivo = obter_tamanho_arquivo(*fp_insere);
    size_t quantidade_registros = tamanho_arquivo / sizeof(registro);

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    if (header.regLidos_insere >= quantidade_registros) {
        printf("Todos os registros foram inseridos.\n");
        return;
    }

    fseek(*fp_insere, header.regLidos_insere * sizeof(registro), SEEK_SET);
    fread(&registro, sizeof(registro), 1, *fp_insere);

    char buffer[512];
    sprintf(buffer, "%s%s#%s#%s#%0.1f#%0.1f#", registro.idAluno, registro.siglaDisc, registro.nomeAluno, registro.nomeDisc, registro.media, registro.freq);

    int tam_reg = strlen(buffer);
    int tam_espaco;

    int BOF_ant_dispo = 8; // Byte offset do terceiro campo do header
    int BOF_atual_dispo;
    int BOF_prox_dispo;

    if (header.BOF_dispo != -1) {
        fseek(fp_arq, header.BOF_dispo, SEEK_SET);
        BOF_atual_dispo = header.BOF_dispo;

        while (true) {
            fread(&tam_espaco, sizeof(int), 1, fp_arq);
            fgetc(fp_arq);
            fread(&BOF_prox_dispo, sizeof(int), 1, fp_arq);

            if (tam_reg <= tam_espaco) {
                fseek(fp_arq, (-1) * sizeof(int) - 1, SEEK_CUR);
                fwrite(buffer, 1, tam_reg, fp_arq);

                if (BOF_ant_dispo == 8) {
                    header.BOF_dispo = BOF_prox_dispo;
                    rewind(fp_arq);
                    fwrite(&header, sizeof(header), 1, fp_arq);
                }

                else {
                    fseek(fp_arq, BOF_ant_dispo + sizeof(int) + 1, SEEK_SET);
                    fwrite(&BOF_prox_dispo, 1, sizeof(int), fp_arq);
                }

                break;
            }

            else {
                if (BOF_prox_dispo == -1) {
                    fseek(fp_arq, 0, SEEK_END);
                    fwrite(&tam_reg, sizeof(int), 1, fp_arq);
                    fwrite(buffer, tam_reg, 1, fp_arq);
                    break;
                }

                else {
                    if (BOF_ant_dispo == 0) {
                        header.BOF_dispo = BOF_atual_dispo;
                        rewind(fp_arq);
                        fwrite(&header, sizeof(header), 1, fp_arq);
                    }

                    else {
                        fseek(fp_arq, BOF_ant_dispo + sizeof(int) + 1, SEEK_SET);
                        fwrite(&BOF_atual_dispo, 1, sizeof(int), fp_arq);
                    }

                    fseek(fp_arq, BOF_atual_dispo + sizeof(int) + 1, SEEK_SET);
                    fwrite(&BOF_prox_dispo, sizeof(int), 1, fp_arq);

                    BOF_ant_dispo = BOF_atual_dispo;
                    BOF_atual_dispo = BOF_prox_dispo;
                    fseek(fp_arq, BOF_prox_dispo, SEEK_SET);
                }
            }
        }
    }

    else {
        fseek(fp_arq, 0, SEEK_END);
        fwrite(&tam_reg, sizeof(int), 1, fp_arq);
        fwrite(buffer, tam_reg, 1, fp_arq);
    }

    header.regLidos_insere++;

    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);

    printf("\nRegistro incluido com sucesso!\n");

    fclose(*fp_insere);
}

void remover(FILE **fp_remove, FILE *fp_arq) {
    if ((*fp_remove = fopen("remove.bin", "r+b")) == NULL) {
        printf("Não foi possivel abrir o arquivo (void remover)");
        return;
    }

    struct Chave key;
    struct Cabecalho header;

    int tam_reg = 0;
    int acum_BOF = sizeof(header);
    int reg_checados = 0;
    char buffer_key[256];
    char buffer_registro[512];

    size_t tamanho_arquivo = obter_tamanho_arquivo(*fp_remove);
    size_t quantidade_registros = tamanho_arquivo / sizeof(key);

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    if (header.regLidos_remove >= quantidade_registros) {
        printf("Todas as remoções foram realizadas.\n");
        return;
    }

    fseek(*fp_remove, header.regLidos_remove * sizeof(key), SEEK_SET);
    fread(&key, sizeof(key), 1, *fp_remove);
    sprintf(buffer_key, "%s%s", key.idAluno, key.siglaDisc);

    while (true) {
        fread(&tam_reg, sizeof(int), 1, fp_arq);
        fread(&buffer_registro, tam_reg, 1, fp_arq);

        if (buffer_registro[0] != buffer_key[0] || buffer_registro[3] != buffer_key[3]) {
            acum_BOF += tam_reg;
            reg_checados++;

            fgetc(fp_arq);
            if (feof(fp_arq)) {
                header.regLidos_remove++;
                rewind(fp_arq);
                fwrite(&header, sizeof(header), 1, fp_arq);

                printf("\nNao ha nenhum registro correspondente a essa chave no arquivo\n");

                fclose(*fp_remove);

                return;
            }

            else
                fseek(fp_arq, -1, SEEK_CUR);
        }

        else
            break;
    }

    fseek(fp_arq, (-1) * tam_reg, SEEK_CUR);
    char estrela = '*';
    fwrite(&estrela, 1, 1, fp_arq);
    fwrite(&header.BOF_dispo, sizeof(int), 1, fp_arq);

    acum_BOF += reg_checados * sizeof(int);

    header.BOF_dispo = acum_BOF;
    header.regLidos_remove++;

    rewind(fp_arq);
    fwrite(&header, sizeof(header), 1, fp_arq);

    printf("\nRegistro excluido com sucesso!\n");

    fclose(*fp_remove);
}

void compactar(FILE *fp_arq) {
    int tam_espaco, tam_registro;
    char buffer[1024];
    char ch_aux;

    struct Cabecalho header;

    FILE *fp_aux;

    if ((fp_aux = fopen("arq_auxiliar.bin", "w+b")) == NULL) {
        printf("Não foi possivel abrir o arquivo (void compactar)");
        return;
    }

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);
    header.BOF_dispo = -1;
    fwrite(&header, sizeof(header), 1, fp_aux);

    while (true) {
        fread(&tam_espaco, sizeof(int), 1, fp_arq);
        ch_aux = fgetc(fp_arq);

        if (ch_aux == '*') {
            fseek(fp_arq, (tam_espaco - 1), SEEK_CUR);
            continue;
        }

        if (ch_aux == EOF) {
            break;
        }

        fseek(fp_arq, -1, SEEK_CUR);
        tam_registro = pega_registro(fp_arq, buffer);

        fwrite(&tam_registro, sizeof(int), 1, fp_aux);
        fwrite(buffer, strlen(buffer), 1, fp_aux);

        fseek(fp_arq, tam_espaco - tam_registro, SEEK_CUR);
        ch_aux = fgetc(fp_arq);

        if (ch_aux != EOF)
            fseek(fp_arq, -1, SEEK_CUR);
        else
            break;
    }

    fclose(fp_arq);
    fclose(fp_aux);

    char arquivo_original[100] = "arq_registros.bin";
    char arquivo_auxiliar[100] = "arq_auxiliar.bin";

    if (remove(arquivo_original) == -1)
        printf("Erro ao excluir arquivo\n");
    if (rename(arquivo_auxiliar, arquivo_original) != 0)
        printf("Erro ao renomear arquivo\n");
    remove(arquivo_auxiliar);

    printf("\nArquivo compactado com sucesso!\n");

    if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL) {
        printf("Não foi possivel abrir o arquivo");
        return;
    }
}

int main() {
    FILE *fp_arq, *fp_insere;
    FILE *fp_remove;

    struct Cabecalho header;

    header.regLidos_insere = 0;
    header.regLidos_remove = 0;
    header.BOF_dispo = -1;

    if (!existeArq()) {
        if ((fp_arq = fopen("arq_registros.bin", "w+b")) == NULL) {
            printf("Não foi possivel abrir o arquivo");
            return 0;
        }

        fwrite(&header, sizeof(header), 1, fp_arq);
        rewind(fp_arq);
    }

    else {
        if ((fp_arq = fopen("arq_registros.bin", "r+b")) == NULL) {
            printf("Não foi possivel abrir o arquivo");
            return 0;
        }
    }

    int op = 0;

    do {
        printf("\n----------------Menu------------------\n");
        printf("1 - Inserir\n");
        printf("2 - Remover\n");
        printf("3 - Compactar\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &op);
        switch (op) {
        case 1:
            printf("Inserindo.\n");
            inserir(&fp_insere, fp_arq);
            break;
        case 2:
            printf("Removendo.\n");
            remover(&fp_remove, fp_arq);
            break;
        case 3:
            printf("Conpactando.\n");
            compactar(fp_arq);
            break;
        case 0:
            printf("Saindo.\n");
            break;
        default:
            printf("Opção inválido.\n");
            break;
        }
    } while (op != 0);
}
