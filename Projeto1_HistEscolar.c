#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<assert.h>
#include<locale.h>

struct Reg {
    char idAluno[3];
    char siglaDisc[3];
    char nomeAluno[50];
    char nomeDisc[50];
    float media;
    float freq;
};

struct Chave {
    char idAluno[3];
    char siglaDisc[3];
};

struct Cabecalho {
    int regLidos_insere;
    int regLidos_remove;
    int BOF_dispo; //BOF=ByteOffSet
};

bool existeArq() {
	if(access("arq_registros.bin", F_OK) == 0)
		return true;
		
	return false;
}

int menu_inicial() {
	int op;
	
	do {
		printf("\n----------------Menu------------------\n");
		printf("Inserir [1]\n");
		printf("Remover [2]\n");
		printf("Compactar [3]\n");
		printf("Sair [4]\n");
		printf("Opcao: ");
		scanf(" %d",&op);
	} while(op != 1 && op != 2 && op != 3 && op != 4);
	
	return op;
}

int pega_registro(FILE *fp, char *buffer) {
    int pipeline_counter = 0;
    int tam_registro = 0;
    int i = 0;

    while(pipeline_counter < 5) {
        buffer[i] = fgetc(fp);
        if(buffer[i] == '|') pipeline_counter++;
        tam_registro++;
        i++;
    }

    buffer[i] = '\0';
    return tam_registro;
}

void inserir(FILE **fp_insere, FILE *fp_arq) {
    if ((*fp_insere = fopen("insere.bin","r+b")) == NULL) {
        printf("Nao foi possivel abrir o arquivo (void inserir)");
        return;
	}
	
	struct Reg registro;
    struct Cabecalho header;

    rewind(fp_arq);
    fread(&header, sizeof(header), 1, fp_arq);

    fseek(*fp_insere, header.regLidos_insere*sizeof(registro), SEEK_SET);
    fread(&registro, sizeof(registro), 1, *fp_insere);
    
    char buffer[512];
    sprintf(buffer, "%s|%s|%s|%s|%0.1f|%0.1f|", registro.idAluno, registro.siglaDisc, registro.nomeAluno, registro.nomeDisc, registro.media, registro.freq);

    int tam_reg = strlen(buffer);
    int tam_espaco;

    int BOF_ant_dispo = 8;  //Byte offset do terceiro campo do header
    int BOF_atual_dispo;
    int BOF_prox_dispo;
    
    if(header.BOF_dispo != -1) {
        fseek(fp_arq, header.BOF_dispo, SEEK_SET);
        BOF_atual_dispo = header.BOF_dispo;

        while(true) {
            fread(&tam_espaco, sizeof(int), 1, fp_arq);
            fgetc(fp_arq);
            fread(&BOF_prox_dispo, sizeof(int), 1, fp_arq);

            if(tam_reg <= tam_espaco) {
                fseek(fp_arq, (-1)*sizeof(int) - 1, SEEK_CUR);
                fwrite(buffer, 1, tam_reg, fp_arq);

                if(BOF_ant_dispo == 8) {
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
                if(BOF_prox_dispo == -1) {
                    fseek(fp_arq, 0, SEEK_END);
                    fwrite(&tam_reg, sizeof(int), 1, fp_arq);
                    fwrite(buffer, tam_reg, 1, fp_arq);
                    break;
                }

                else {
                    if(BOF_ant_dispo == 8) {
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

    printf("\nRegistro incluído com sucesso!\n");

    fclose(*fp_insere);
}

int main () {
    setlocale(LC_ALL,"");
    FILE *fp_arq, *fp_insere, *fp_remove;

    struct Cabecalho header;

    header.regLidos_insere = 0;
    header.regLidos_remove = 0;
    header.BOF_dispo = -1;
    
    if(!existeArq()) {
        if ((fp_arq = fopen("arq_registros.bin","w+b")) == NULL) {
	    	printf("Não foi possível abrir o arquivo");
		    return 0;
	    }

        fwrite(&header, sizeof(header), 1, fp_arq);
        rewind(fp_arq);
    }

    else {
        if((fp_arq = fopen("arq_registros.bin","r+b")) == NULL) {
	    	printf("Não foi possível abrir o arquivo");
		    return 0;
	    }
    }

    int op = 0;

    while(op < 4) {
        op = menu_inicial();

        switch(op) {
            case 1: inserir(&fp_insere, fp_arq);
                    break;
            case 2: //remover(&fp_remove, fp_arq);
                    break;
            case 3: //compactar(fp_arq);
                    break;
            default: break;
        }
    }
}
