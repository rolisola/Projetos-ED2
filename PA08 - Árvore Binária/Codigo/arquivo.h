#ifndef ARQUIVO_H_INCLUDED
#define ARQUIVO_H_INCLUDED

int validar_modo(const char *modo);
FILE *abrir_arquivo(const char *nome_arquivo, const char *modo);
FILE *criar_arquivo(const char *nome_arquivo, const char *modo);
FILE *abrir_criar_arquivo(const char *nome_arquivo, const char *modo);
int fechar_arquivo(FILE *arquivo);
size_t obter_tamanho_arquivo(FILE *arquivo);

#endif // ARQUIVO_H_INCLUDED
