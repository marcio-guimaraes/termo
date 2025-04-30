#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_TENTATIVAS 6
#define TAMANHO_PALAVRA 5
#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

char palavraCerta[MAX_WORD_LENGTH];
GtkWidget *grid;
int tentativaAtual = 0;

void carregarPalavras(const char *nomeArquivo, char palavras[][MAX_WORD_LENGTH], int *numPalavras) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o dicionário");
        exit(1);
    }

    *numPalavras = 0;
    while (*numPalavras < MAX_WORDS && fgets(palavras[*numPalavras], MAX_WORD_LENGTH, arquivo)) {
        palavras[*numPalavras][strcspn(palavras[*numPalavras], "\n")] = '\0';  // remove \n
        (*numPalavras)++;
    }
    fclose(arquivo);
}

void selecionarPalavraAleatoria(char palavras[][MAX_WORD_LENGTH], int numPalavras, char *palavraSelecionada) {
    srand(time(NULL));
    int index = rand() % numPalavras;
    strcpy(palavraSelecionada, palavras[index]);
    for (int i = 0; palavraSelecionada[i]; i++)
        palavraSelecionada[i] = toupper(palavraSelecionada[i]);
}

void aplicarCor(GtkWidget *widget, const char *corCSS) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();

    char css[128];
    snprintf(css, sizeof(css), "* { background-color: %s; color: white; font-weight: bold; }", corCSS);

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void on_submit_clicked(GtkButton *botao, gpointer entryPtr) {
    if (tentativaAtual >= MAX_TENTATIVAS) return;

    const char *entrada = gtk_entry_get_text(GTK_ENTRY(entryPtr));
    if (strlen(entrada) != TAMANHO_PALAVRA) return;

    char tentativa[6];
    strncpy(tentativa, entrada, TAMANHO_PALAVRA);
    tentativa[TAMANHO_PALAVRA] = '\0';

    for (int i = 0; tentativa[i]; i++)
        tentativa[i] = toupper(tentativa[i]);

    int letrasUsadas[5] = {0};

    for (int i = 0; i < TAMANHO_PALAVRA; i++) {
        GtkWidget *label = gtk_label_new(NULL);
        char letraStr[2] = {tentativa[i], '\0'};
        gtk_label_set_text(GTK_LABEL(label), letraStr);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_vexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(grid), label, i, tentativaAtual, 1, 1);

        if (tentativa[i] == palavraCerta[i]) {
            aplicarCor(label, "#228B22");  // verde
            letrasUsadas[i] = 1;
        } else {
            int achou = 0;
            for (int j = 0; j < TAMANHO_PALAVRA; j++) {
                if (tentativa[i] == palavraCerta[j] && !letrasUsadas[j]) {
                    achou = 1;
                    letrasUsadas[j] = 1;
                    break;
                }
            }
            aplicarCor(label, achou ? "#DAA520" : "#2F4F4F");  // amarelo ou cinza escuro
        }
    }

    gtk_widget_show_all(grid);
    tentativaAtual++;

    gtk_entry_set_text(GTK_ENTRY(entryPtr), "");

    if (strcmp(tentativa, palavraCerta) == 0 || tentativaAtual == MAX_TENTATIVAS) {
        char mensagem[128];
        if (strcmp(tentativa, palavraCerta) == 0)
            sprintf(mensagem, "Parabéns! Você acertou!");
        else
            sprintf(mensagem, "A palavra era: %s", palavraCerta);

        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", mensagem);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    char palavras[MAX_WORDS][MAX_WORD_LENGTH];
    int numPalavras;
    carregarPalavras("dicionario.txt", palavras, &numPalavras);
    selecionarPalavraAleatoria(palavras, numPalavras, palavraCerta);

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Termo - GTK");
    gtk_window_set_default_size(GTK_WINDOW(janela), 400, 400);
    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *caixaVertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(janela), caixaVertical);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(caixaVertical), grid, TRUE, TRUE, 5);

    GtkWidget *entrada = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entrada), TAMANHO_PALAVRA);
    gtk_box_pack_start(GTK_BOX(caixaVertical), entrada, FALSE, FALSE, 5);

    GtkWidget *botao = gtk_button_new_with_label("Enviar");
    gtk_box_pack_start(GTK_BOX(caixaVertical), botao, FALSE, FALSE, 5);

    g_signal_connect(botao, "clicked", G_CALLBACK(on_submit_clicked), entrada);

    gtk_widget_show_all(janela);
    gtk_main();
    return 0;
}
