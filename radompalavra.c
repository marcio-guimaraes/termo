#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 15000
#define MAX_WORD_LENGTH 100

// Função para carregar palavras de um arquivo
int carregarPalavras(const char* nomeArquivo, char palavras[][MAX_WORD_LENGTH], int maxPalavras) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
        return 0;
    }

    int count = 0;
    while (count < maxPalavras && fgets(palavras[count], MAX_WORD_LENGTH, arquivo)) {
        // Remove o caractere de nova linha
        palavras[count][strcspn(palavras[count], "\n")] = '\0';
        count++;
    }

    fclose(arquivo);
    return count;
}

// Função para selecionar uma palavra aleatória
void selecionarPalavraAleatoria(char palavras[][MAX_WORD_LENGTH], int numPalavras, char* palavraSelecionada) {
    srand(time(NULL));
    int index = rand() % numPalavras;
    strcpy(palavraSelecionada, palavras[index]);
}
