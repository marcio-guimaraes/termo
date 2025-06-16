#include "jogo_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void normalizar_palavra(const char *input, char *output) {
    gchar *normalized_string = g_utf8_normalize(input, -1, G_NORMALIZE_NFD);
    gchar *out_ptr = output;
    const gchar *p = normalized_string;
    while (*p) {
        gunichar uc = g_utf8_get_char(p);
        if (!g_unichar_ismark(uc)) {
            gunichar upper_char = g_unichar_toupper(uc);
            out_ptr += g_unichar_to_utf8(upper_char, out_ptr);
        }
        p = g_utf8_next_char(p);
    }
    *out_ptr = '\0';
    g_free(normalized_string);
}

int carregar_palavras_do_arquivo(const char *nomeArquivo, char palavras_array[][MAX_WORD_LENGTH], int max_palavras) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", nomeArquivo);
        return 0; // Retorna 0 em caso de erro
    }
    
    int numPalavras_ptr = 0;
    char buffer[MAX_WORD_LENGTH];
    while (numPalavras_ptr < max_palavras && fgets(buffer, MAX_WORD_LENGTH, arquivo)) {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // Remove nova linha
        if (g_utf8_strlen(buffer, -1) == TAMANHO_PALAVRA) {
            strcpy(palavras_array[numPalavras_ptr], buffer);
            numPalavras_ptr++;
        }
    }
    fclose(arquivo);
    return numPalavras_ptr;
}

void selecionar_palavra_aleatoria(char palavras_param[][MAX_WORD_LENGTH], int numPalavras_param, char *palavraSelecionada) {
    // srand() deve ser chamado uma vez no início do jogo (ex: no main ou iniciar_jogo_termo)
    if (numPalavras_param > 0) {
        int index = rand() % numPalavras_param;
        strcpy(palavraSelecionada, palavras_param[index]);
    }
}

void aplicar_cor_widget(GtkWidget *widget, const char *corCSS) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();
    char css[128];
    snprintf(css, sizeof(css), "* { background-color: %s; color: white; font-weight: bold; }", corCSS);
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}