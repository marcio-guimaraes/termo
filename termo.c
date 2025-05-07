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
GtkWidget *tecladoGrid;
GtkWidget *teclas[26]; // Para armazenar os botões das letras
int mapeamentoTeclas[26]; // Mapeia letras A-Z para índices no layout QWERTY
int tentativaAtual = 0;

void carregarPalavras(const char *nomeArquivo, char palavras[][MAX_WORD_LENGTH], int *numPalavras)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir o dicionário");
        exit(1);
    }

    *numPalavras = 0;
    while (*numPalavras < MAX_WORDS && fgets(palavras[*numPalavras], MAX_WORD_LENGTH, arquivo))
    {
        palavras[*numPalavras][strcspn(palavras[*numPalavras], "\n")] = '\0'; // remove \n
        (*numPalavras)++;
    }
    fclose(arquivo);
}

void selecionarPalavraAleatoria(char palavras[][MAX_WORD_LENGTH], int numPalavras, char *palavraSelecionada)
{
    srand(time(NULL));
    int index = rand() % numPalavras;
    strcpy(palavraSelecionada, palavras[index]);
    for (int i = 0; palavraSelecionada[i]; i++)
        palavraSelecionada[i] = toupper(palavraSelecionada[i]);
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
    gtk_box_pack_end(GTK_BOX(caixaVertical), tecladoGrid, FALSE, FALSE, 5); // Posiciona o teclado na parte de baixo

    const char *layout = "QWERTYUIOPASDFGHJKLZXCVBNM";
    int linha = 0, coluna = 0;

    for (int i = 0; i < 26; i++)
    {
        char letra[2] = {layout[i], '\0'};
        GtkWidget *label = gtk_label_new(letra); // Substitui GtkButton por GtkLabel
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_vexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

        gtk_grid_attach(GTK_GRID(tecladoGrid), label, coluna, linha, 1, 1);
        teclas[i] = label; // Armazena o GtkLabel no array teclas

        // Preenche o mapeamento para a letra atual
        mapeamentoTeclas[layout[i] - 'A'] = i;

        coluna++;
        if ((i == 9) || (i == 18)) // Quebra de linha após 10 e 9 teclas
        {
            linha++;
            coluna = 0;
        }
    }
}

void atualizarTeclado(const char *tentativa, const char *palavraCerta)
{
    int letrasUsadas[TAMANHO_PALAVRA] = {0}; // Para rastrear quais letras da palavra certa já foram usadas

    // Primeiro, marque as letras que estão na posição correta (verde)
    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        if (tentativa[i] == palavraCerta[i])
        {
            aplicarCor(teclas[indice], "#32CD32"); // Verde forte
            letrasUsadas[i] = 1; // Marca a letra como usada
        }
    }

    // Depois, marque as letras que estão na palavra, mas em posições erradas (amarelo)
    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        if (tentativa[i] != palavraCerta[i]) // Ignora as já marcadas como corretas
        {
            int achou = 0;
            for (int j = 0; j < TAMANHO_PALAVRA; j++)
            {
                if (tentativa[i] == palavraCerta[j] && !letrasUsadas[j])
                {
                    achou = 1;
                    letrasUsadas[j] = 1; // Marca a letra como usada
                    break;
                }
            }
            if (achou)
            {
                aplicarCor(teclas[indice], "#FFD700"); // Amarelo
            }
        }
    }

    // Por último, marque as letras que não estão na palavra (cinza)
    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        int achou = 0;

        // Verifica se a letra está na palavra certa
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
        tentativa[i] = toupper(tentativa[i]);

    atualizarTeclado(tentativa, palavraCerta);

    int letrasUsadas[5] = {0};

    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        GtkWidget *label = gtk_label_new(NULL);
        char letraStr[2] = {tentativa[i], '\0'};
        gtk_label_set_text(GTK_LABEL(label), letraStr);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_vexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
        gtk_grid_attach(GTK_GRID(grid), label, i, tentativaAtual, 1, 1);

        if (tentativa[i] == palavraCerta[i])
        {
            aplicarCor(label, "#32CD32"); // Verde forte
            letrasUsadas[i] = 1;
        }
        else
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
            aplicarCor(label, achou ? "#FFD700" : "#696969"); // Amarelo forte ou cinza escuro
        }
    }

    gtk_widget_show_all(grid);
    tentativaAtual++;

    gtk_entry_set_text(GTK_ENTRY(entryPtr), "");

    if (strcmp(tentativa, palavraCerta) == 0 || tentativaAtual == MAX_TENTATIVAS)
    {
        char mensagem[128];
        if (strcmp(tentativa, palavraCerta) == 0)
            sprintf(mensagem, "Parabéns! Você acertou!");
        else
            sprintf(mensagem, "A palavra era: %s", palavraCerta);

        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", mensagem);

        // Adiciona o estilo CSS ao diálogo
        GtkCssProvider *css_provider = gtk_css_provider_new();
        const char *css_data =
            "dialog { background-color: #222222; color: white; } "
            "label { color: white; font-weight: bold; font-size: 20px; } "
            "button { background-color: #222222; color: white; border: 2px solid white; font-size: 15px; }";
        gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);

        GtkStyleContext *context = gtk_widget_get_style_context(dialog);
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

        g_object_unref(css_provider);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    char palavras[MAX_WORDS][MAX_WORD_LENGTH];
    int numPalavras;
    carregarPalavras("dicionario.txt", palavras, &numPalavras);
    selecionarPalavraAleatoria(palavras, numPalavras, palavraCerta);

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Termo - GTK");

    // Ajusta a altura da janela para caber 6 tentativas
    int altura_por_tentativa = 60;                                  // Altura aproximada para cada linha de tentativa
    int altura_total = altura_por_tentativa * MAX_TENTATIVAS + 150; // Inclui espaço para entrada e botão
    gtk_window_set_default_size(GTK_WINDOW(janela), 400, altura_total);

    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);

    if (monitor)
    {
        GdkRectangle geometry;
        gdk_monitor_get_geometry(monitor, &geometry);

        int screen_width = geometry.width;
        int screen_height = geometry.height;

        // Calcula as coordenadas para centralizar horizontalmente e posicionar um pouco acima verticalmente
        int pos_x = (screen_width - 400) / 2;  // 400 é a largura da janela
        int pos_y = (screen_height - 400) / 3; // 400 é a altura da janela, ajustado para ficar acima

        gtk_window_move(GTK_WINDOW(janela), pos_x, pos_y);
    }

    // Define o estilo CSS para fundo roxo, cores mais fortes, letras maiores e padding
    const char *css_data =
        "window { background-color: #222222; color: white; } "                                           // Fundo roxo
        "label { color: white; font-weight: bold; font-size: 25px; padding: 10px; } "                    // Letras maiores com padding
        "entry { background-color: #222222; color: white; border: 3px solid white; font-size: 25px; } "  // Letras maiores no campo de entrada
        "button { background-color: #222222; color: white; border: 3px solid white; font-size: 25px; }"; // Letras maiores nos botões

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(janela);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(css_provider);

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

    inicializarTeclado(caixaVertical); // Adiciona o teclado virtual na parte de baixo

    g_signal_connect(botao, "clicked", G_CALLBACK(on_submit_clicked), entrada);
    g_signal_connect(entrada, "activate", G_CALLBACK(on_submit_clicked), entrada);

    gtk_widget_show_all(janela);
    gtk_main();
    return 0;
}
