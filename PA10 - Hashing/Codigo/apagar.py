import os

# Especifique o caminho do arquivo
caminho_arquivo1 = 'auxiliar.bin'
caminho_arquivo2 = 'dados.bin'
caminho_arquivo3 = 'arvore.bin'

# Verifica se o arquivo existe
if os.path.isfile(caminho_arquivo1):
    os.remove(caminho_arquivo1)  # Apaga o arquivo
    print(f'Arquivo {caminho_arquivo1} foi apagado.')
else:
    print(f'Arquivo {caminho_arquivo1} não encontrado.')

# Verifica se o arquivo existe
if os.path.isfile(caminho_arquivo2):
    os.remove(caminho_arquivo2)  # Apaga o arquivo
    print(f'Arquivo {caminho_arquivo2} foi apagado.')
else:
    print(f'Arquivo {caminho_arquivo2} não encontrado.')

# Verifica se o arquivo existe
if os.path.isfile(caminho_arquivo3):
    os.remove(caminho_arquivo3)  # Apaga o arquivo
    print(f'Arquivo {caminho_arquivo3} foi apagado.')
else:
    print(f'Arquivo {caminho_arquivo3} não encontrado.')

