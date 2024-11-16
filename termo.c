#include <stdio.h>
#include <windows.h>  // Biblioteca do Windows para manipular a cor de fundo
#include <string.h>
#include "radompalavra.c"  // Incluindo o código para seleção de palavras

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

// Função para configurar a cor do texto e do fundo no console
void setConsoleColor(int textColor, int bgColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, textColor | (bgColor << 4));
}

// Função para resetar a cor do console
void resetConsoleColor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);  // Cor padrão: texto branco em fundo preto
}

void verificarLetras(char* tentativa, char* palavraCerta, int* verificador1)
{
    for (int j = 0; j < 5; j++)
    {
        if (tentativa[j] == palavraCerta[j]) 
        {
            setConsoleColor(0, 2);  // Define a cor do fundo como verde
            printf("%2c ", tentativa[j]);
            verificador1[j] = 1;  // Marca a letra como verificada na posição correta
        }
        else 
        {
            int verificador2 = 0;  // Variável para verificar letras presentes na palavra certa, mas em posição errada

            for (int k = 0; k < 5; k++) 
            {
                if (tentativa[j] == palavraCerta[k] && verificador1[k] == 0) 
                {
                    verificador2 = 1;
                    verificador1[k] = 1;  // Marca a letra como já verificada
                    break;
                }
            }

            if (verificador2 > 0) 
            {
                setConsoleColor(0, 6);  // Define a cor do fundo como amarelo
                printf("%2c ", tentativa[j]);
            }
            else 
            {
                setConsoleColor(7, 0);  // Define a cor do fundo como preto
                printf("%2c ", tentativa[j]);
            }
        }
    }
}

int main()
{
    // Palavra correta que o jogador deve adivinhar
    char palavraCerta[MAX_WORD_LENGTH];
    
    // Carrega a palavra aleatória
    char palavras[MAX_WORDS][MAX_WORD_LENGTH];
    int numPalavras = carregarPalavras("../dicionario.txt", palavras, MAX_WORDS);
    if (numPalavras == 0) {
        fprintf(stderr, "Nenhuma palavra carregada\n");
        return 1;
    }
    
    selecionarPalavraAleatoria(palavras, numPalavras, palavraCerta);

    // Laço para permitir até 6 tentativas de adivinhação
    for (int i = 0; i < 6; i++) 
    {
        // Array para marcar quais letras já foram verificadas
        int verificador1[5] = {0};
        // Array para armazenar a palavra de tentativa do jogador
        char tentativa[8];
        // Lê a tentativa do jogador
        scanf("%s", tentativa);
        strupr(tentativa); // Deixa a palavra com letras maiusculas

        // Chama a função para verificar e imprimir letras com fundo verde ou amarelo
        verificarLetras(tentativa, palavraCerta, verificador1);

        resetConsoleColor();  // Reseta a cor do console para o padrão
        printf("\n<---------------------------->\n");  // Separador entre as tentativas
    }

    printf("A palavra certa era: %s\n", palavraCerta);

    return 0;
}
