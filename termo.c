#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// --- Constantes do Jogo ---
#define MAX_TENTATIVAS 6
#define TAMANHO_PALAVRA 5
#define MAX_WORD_LENGTH 100
// IMPORTANTE: Ajuste estes valores conforme o tamanho dos seus dicionários
#define MAX_WORDS_SORTEIO 1000      // Para o "dicionario.txt"
#define MAX_WORDS_VALIDACAO 30000   // Para o "palavras_existentes.txt"


// --- Variáveis Globais ---
char palavraCerta[MAX_WORD_LENGTH];
GtkWidget *grid;
GtkWidget *tecladoGrid;
GtkWidget *teclas[26];
GtkWidget *labelTentativas;
GtkWidget *labelMensagem; 

// Array 2D para guardar os labels da grelha
GtkWidget *grid_tiles[MAX_TENTATIVAS][TAMANHO_PALAVRA];

int mapeamentoTeclas[26];
int tentativaAtual = 0;

// Dicionário para sortear a palavra secreta
char palavras_sorteio[MAX_WORDS_SORTEIO][MAX_WORD_LENGTH];
int numPalavras_sorteio;

// Dicionário com todas as palavras válidas para validação
char palavras_existentes[MAX_WORDS_VALIDACAO][MAX_WORD_LENGTH];
int numPalavras_existentes;
// --- Fim das Variáveis Globais ---


// Declaração de funções
void mostrarTelaVitoria(GtkWidget *parent);

gboolean limpar_label_mensagem(gpointer data) {
    gtk_label_set_text(GTK_LABEL(data), "");
    return G_SOURCE_REMOVE;
}

gboolean palavra_existe_no_dicionario(const char *palavra) {
    for (int i = 0; i < numPalavras_existentes; i++) {
        if (strcmp(palavra, palavras_existentes[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void carregarPalavras(const char *nomeArquivo, char palavras_array[][MAX_WORD_LENGTH], int *numPalavras_ptr)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo)
    {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", nomeArquivo);
        exit(1);
    }

    *numPalavras_ptr = 0;
    int max_words = (palavras_array == palavras_sorteio) ? MAX_WORDS_SORTEIO : MAX_WORDS_VALIDACAO;
    
    char buffer[MAX_WORD_LENGTH];
    while (*numPalavras_ptr < max_words && fgets(buffer, MAX_WORD_LENGTH, arquivo))
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; 
        if (strlen(buffer) == TAMANHO_PALAVRA)
        {
            strcpy(palavras_array[*numPalavras_ptr], buffer);
            (*numPalavras_ptr)++;
        }
    }
    fclose(arquivo);
}

void selecionarPalavraAleatoria(char palavras_param[][MAX_WORD_LENGTH], int numPalavras_param, char *palavraSelecionada)
{
    srand(time(NULL));
    int index = rand() % numPalavras_param;
    strcpy(palavraSelecionada, palavras_param[index]);
}

void aplicarCor(GtkWidget *widget, const char *corCSS)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();

    char css[128];
    snprintf(css, sizeof(css), "* { background-color: %s; color: white; font-weight: bold; }", corCSS);

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void inicializarTeclado(GtkWidget *caixaVertical)
{
    tecladoGrid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(tecladoGrid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(tecladoGrid), 5);
    gtk_box_pack_end(GTK_BOX(caixaVertical), tecladoGrid, FALSE, FALSE, 5);

    const char *layout = "QWERTYUIOPASDFGHJKLZXCVBNM";
    int linha = 0, coluna = 0;

    for (int i = 0; i < 26; i++)
    {
        char letra[2] = {layout[i], '\0'};
        GtkWidget *label = gtk_label_new(letra);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_vexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        
        GtkStyleContext *context = gtk_widget_get_style_context(label);
        gtk_style_context_add_class(context, "tecla-teclado");

        gtk_grid_attach(GTK_GRID(tecladoGrid), label, coluna, linha, 1, 1);
        teclas[i] = label;

        mapeamentoTeclas[layout[i] - 'A'] = i;

        coluna++;
        if ((i == 9) || (i == 18))
        {
            linha++;
            coluna = 0;
        }
    }
}

void atualizarTeclado(const char *tentativa, const char *palavraCerta)
{
    int letrasUsadas[TAMANHO_PALAVRA] = {0};

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        if (tentativa[i] == palavraCerta[i])
        {
            aplicarCor(teclas[indice], "#32CD32"); // Verde
            letrasUsadas[i] = 1;
        }
    }

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        if (tentativa[i] != palavraCerta[i])
        {
            int achou = 0;
            for (int j = 0; j < TAMANHO_PALAVRA; j++)
            {
                if (tentativa[i] == palavraCerta[j] && !letrasUsadas[j])
                {
                    achou = 1;
                    letrasUsadas[j] = 1;
                    break;
                }
            }
            if (achou)
            {
                aplicarCor(teclas[indice], "#FFD700"); // Amarelo
            }
        }
    }

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        int achou = 0;

        for (int j = 0; j < TAMANHO_PALAVRA; j++)
        {
            if (tentativa[i] == palavraCerta[j])
            {
                achou = 1;
                break;
            }
        }

        if (!achou)
        {
            aplicarCor(teclas[indice], "#696969"); // Cinza
        }
    }
}

void on_submit_clicked(GtkButton *botao, gpointer entryPtr)
{
    if (tentativaAtual >= MAX_TENTATIVAS)
        return;

    const char *entrada = gtk_entry_get_text(GTK_ENTRY(entryPtr));
    if (strlen(entrada) != TAMANHO_PALAVRA)
        return;

    char tentativa[6];
    strncpy(tentativa, entrada, TAMANHO_PALAVRA);
    tentativa[TAMANHO_PALAVRA] = '\0';

    for (int i = 0; tentativa[i]; i++)
        tentativa[i] = toupper((unsigned char)tentativa[i]);

    if (!palavra_existe_no_dicionario(tentativa)) {
        gtk_label_set_text(GTK_LABEL(labelMensagem), "Palavra inválida!");
        g_timeout_add(2000, limpar_label_mensagem, labelMensagem);
        gtk_entry_set_text(GTK_ENTRY(entryPtr), "");
        return;
    }
    
    atualizarTeclado(tentativa, palavraCerta);

    int usadas_palavra[TAMANHO_PALAVRA] = {0};

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        GtkWidget *label = grid_tiles[tentativaAtual][i];
        
        char letraStr[2] = {tentativa[i], '\0'};
        gtk_label_set_text(GTK_LABEL(label), letraStr);

        if (tentativa[i] == palavraCerta[i])
        {
            aplicarCor(label, "#32CD32");
            usadas_palavra[i] = 1;
        }
    }

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        GtkWidget *label = grid_tiles[tentativaAtual][i];

        if (tentativa[i] == palavraCerta[i])
            continue;

        int achou = 0;
        for (int j = 0; j < TAMANHO_PALAVRA; j++)
        {
            if (!usadas_palavra[j] && tentativa[i] == palavraCerta[j])
            {
                achou = 1;
                usadas_palavra[j] = 1;
                break;
            }
        }
        aplicarCor(label, achou ? "#FFD700" : "#696969");
    }

    tentativaAtual++;

    char texto_tentativas[50];
    int tentativas_restantes = MAX_TENTATIVAS - tentativaAtual;
    sprintf(texto_tentativas, "Tentativas restantes: %d", tentativas_restantes);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_tentativas);

    gtk_entry_set_text(GTK_ENTRY(entryPtr), "");

    if (strcmp(tentativa, palavraCerta) == 0)
    {
        mostrarTelaVitoria(gtk_widget_get_toplevel(GTK_WIDGET(botao)));
    }
    else if (tentativaAtual == MAX_TENTATIVAS)
    {
        char mensagem[128];
        sprintf(mensagem, "A palavra era: %s", palavraCerta);

        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(botao))),
            GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "%s", mensagem);
        gtk_window_set_title(GTK_WINDOW(dialog), "Fim de Jogo");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_window_close(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(botao))));
    }
}


void mostrarTelaVitoria(GtkWidget *parent)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Parabéns! A palavra era %s", palavraCerta);
    gtk_window_set_title(GTK_WINDOW(dialog), "Vitória!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gtk_window_close(GTK_WINDOW(parent));
}


void iniciar_jogo_termo(int argc, char *argv[])
{
    tentativaAtual = 0;
    gtk_init(&argc, &argv);

    carregarPalavras("palavras.txt", palavras_sorteio, &numPalavras_sorteio);
    if (numPalavras_sorteio == 0) { exit(1); }
    
    carregarPalavras("palavras.txt", palavras_existentes, &numPalavras_existentes);
    if (numPalavras_existentes == 0) { exit(1); }

    selecionarPalavraAleatoria(palavras_sorteio, numPalavras_sorteio, palavraCerta);

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Termo - GTK");
    gtk_window_set_default_size(GTK_WINDOW(janela), 400, 700);
    gtk_window_set_position(GTK_WINDOW(janela), GTK_WIN_POS_CENTER);

    const char *css_data =
        ".grid-tile { font-size: 25px; font-family: monospace; border: 2px solid #ddd; background-color: white; }"
        ".tecla-teclado { font-size: 20px; font-family: monospace; border-radius: 5px; background-color: #d0d0d0; border: 1px solid #a0a0a0; padding: 5px; }"
        "entry { font-size: 25px; font-family: monospace; } "
        "button { font-size: 25px; font-family: monospace; }"
        "#tentativas, #mensagem { border: none; background-color: transparent; }"
        "#mensagem { color: red; font-size: 16px; font-weight: bold; }";

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *caixaVertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(janela), caixaVertical);

    labelTentativas = gtk_label_new(NULL);
    gtk_widget_set_name(labelTentativas, "tentativas");
    char texto_inicial[50];
    sprintf(texto_inicial, "Tentativas restantes: %d", MAX_TENTATIVAS);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_inicial);
    gtk_box_pack_start(GTK_BOX(caixaVertical), labelTentativas, FALSE, FALSE, 5);
    
    labelMensagem = gtk_label_new("");
    gtk_widget_set_name(labelMensagem, "mensagem");
    gtk_box_pack_start(GTK_BOX(caixaVertical), labelMensagem, FALSE, FALSE, 5);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(caixaVertical), grid, TRUE, TRUE, 5);

    for (int i = 0; i < MAX_TENTATIVAS; i++) {
        for (int j = 0; j < TAMANHO_PALAVRA; j++) {
            GtkWidget *label = gtk_label_new("");
            gtk_widget_set_hexpand(label, TRUE);
            gtk_widget_set_vexpand(label, TRUE);
            gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
            
            GtkStyleContext *context = gtk_widget_get_style_context(label);
            gtk_style_context_add_class(context, "grid-tile");

            gtk_grid_attach(GTK_GRID(grid), label, j, i, 1, 1);
            grid_tiles[i][j] = label;
        }
    }

    GtkWidget *entrada = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entrada), TAMANHO_PALAVRA);
    gtk_entry_set_alignment(GTK_ENTRY(entrada), 0.5);
    gtk_box_pack_start(GTK_BOX(caixaVertical), entrada, FALSE, FALSE, 5);

    GtkWidget *botao = gtk_button_new_with_label("Enviar");
    gtk_box_pack_start(GTK_BOX(caixaVertical), botao, FALSE, FALSE, 5);

    inicializarTeclado(caixaVertical);

    g_signal_connect(botao, "clicked", G_CALLBACK(on_submit_clicked), entrada);
    g_signal_connect(entrada, "activate", G_CALLBACK(on_submit_clicked), entrada);

    gtk_widget_show_all(janela);
    gtk_main();
}