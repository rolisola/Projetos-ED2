#include <stdio.h>
#include "registro.h"

int main() {
    int opcao = -1;
    const char *arquivo_Historico = "historico.bin";
    FILE *arquivo = carregar_Historico(arquivo_Historico);
    carregar_insere();




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
