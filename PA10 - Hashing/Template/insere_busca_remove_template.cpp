#include<stdio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct hist {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[6] = { {"111", "001", "Nome-111", "Disc-001", 8.9, 80.3},
                 {"222", "001", "Nome-222", "Disc-001", 3.3, 72.3},
                 {"333", "003", "Nome-333", "Disc-003", 3.3, 72.3},
                 {"111", "001", "Nome-111", "Disc-001", 3.3, 72.3},
                 {"222", "003", "Nome-222", "Disc-001", 3.3, 72.3},
                 {"333", "002", "Nome-333", "Disc-002", 9.7, 73.7}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_b[2] = { {"001","001"},
                   {"001","002"}};
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
    
    //////////////////////////////
    struct remove {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_r[1] = { {"001","001"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

