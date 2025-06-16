// jogo_utils.h
#ifndef JOGO_UTILS_H
#define JOGO_UTILS_H

#include <gtk/gtk.h>
#include <glib.h>

// --- Constantes ---
#define TAMANHO_PALAVRA 5
#define MAX_WORD_LENGTH 100

/**
 * @brief Normaliza uma string, removendo acentos e a convertendo para maiúsculas.
 * @param input A string de entrada.
 * @param output O buffer de saída para a string normalizada.
 */
void normalizar_palavra(const char *input, char *output);

/**
 * @brief Carrega palavras de um arquivo de texto para um array.
 * Apenas palavras com o tamanho definido por TAMANHO_PALAVRA são carregadas.
 * @param nomeArquivo O caminho do arquivo a ser lido.
 * @param palavras_array O array de strings que armazenará as palavras.
 * @param max_palavras O tamanho máximo do array de palavras.
 * @return O número de palavras carregadas com sucesso.
 */
int carregar_palavras_do_arquivo(const char *nomeArquivo, char palavras_array[][MAX_WORD_LENGTH], int max_palavras);

/**
 * @brief Seleciona uma palavra aleatória de um array de palavras.
 * @param palavras_param O array de palavras de onde selecionar.
 * @param numPalavras_param O número total de palavras no array.
 * @param palavraSelecionada O buffer para armazenar a palavra selecionada.
 */
void selecionar_palavra_aleatoria(char palavras_param[][MAX_WORD_LENGTH], int numPalavras_param, char *palavraSelecionada);

/**
 * @brief Aplica uma cor de fundo e de texto a um widget GTK usando CSS.
 * @param widget O GtkWidget a ser estilizado.
 * @param corCSS A cor de fundo no formato CSS (ex: "#FF5733").
 */
void aplicar_cor_widget(GtkWidget *widget, const char *corCSS);

#endif // JOGO_UTILS_H