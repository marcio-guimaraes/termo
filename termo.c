#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Define constantes do jogo
#define MAX_TENTATIVAS 6
#define TAMANHO_PALAVRA 5
#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

// Variáveis globais
char palavraCerta[MAX_WORD_LENGTH]; // Palavra correta a ser adivinhada
GtkWidget *grid;                    // Grid principal para as tentativas
GtkWidget *tecladoGrid;             // Grid do teclado virtual
GtkWidget *teclas[26];              // Array para armazenar os botões das letras
GtkWidget *labelTentativas;         // <-- 1. DECLARAÇÃO DO NOVO LABEL
int mapeamentoTeclas[26];           // Mapeia letras A-Z para índices no layout QWERTY
int tentativaAtual = 0;             // Número da tentativa atual

void mostrarTelaVitoria(GtkWidget *parent);

// Carrega as palavras do arquivo de dicionário
void carregarPalavras(const char *nomeArquivo, char palavras[][MAX_WORD_LENGTH], int *numPalavras)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir o dicionário");
        exit(1);
    }

    *numPalavras = 0;
    char buffer[MAX_WORD_LENGTH];
    while (fgets(buffer, MAX_WORD_LENGTH, arquivo))
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // remove \n e \r
        if (strlen(buffer) == TAMANHO_PALAVRA)  // só aceita palavras com 5 letras
        {
            strcpy(palavras[*numPalavras], buffer);
            (*numPalavras)++;
        }
    }
    fclose(arquivo);
}

// Seleciona uma palavra aleatória do dicionário
void selecionarPalavraAleatoria(char palavras[][MAX_WORD_LENGTH], int numPalavras, char *palavraSelecionada)
{
    srand(time(NULL));
    int index = rand() % numPalavras;
    strcpy(palavraSelecionada, palavras[index]);
    // Converte para maiúsculas
    for (int i = 0; palavraSelecionada[i]; i++)
        palavraSelecionada[i] = toupper((unsigned char)palavraSelecionada[i]);
}

// Aplica uma cor de fundo e texto a um widget usando CSS
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

// Inicializa o teclado virtual na interface
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
        GtkWidget *label = gtk_label_new(letra); // Usa GtkLabel para as teclas
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

// Atualiza as cores das teclas do teclado virtual conforme a tentativa
void atualizarTeclado(const char *tentativa, const char *palavraCerta)
{
    int letrasUsadas[TAMANHO_PALAVRA] = {0}; // Para rastrear quais letras da palavra certa já foram usadas

    // Primeiro, marca as letras na posição correta (verde)
    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        int indice = mapeamentoTeclas[toupper(tentativa[i]) - 'A'];
        if (tentativa[i] == palavraCerta[i])
        {
            aplicarCor(teclas[indice], "#32CD32"); // Verde forte
            letrasUsadas[i] = 1;                   // Marca a letra como usada
        }
    }

    // Depois, marca as letras que estão na palavra, mas em posições erradas (amarelo)
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

    // Por último, marca as letras que não estão na palavra (cinza)
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

// Callback do botão "Enviar" e do Enter no campo de entrada
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

    atualizarTeclado(tentativa, palavraCerta);

    int letrasUsadas[5] = {0};

    // Mostra a tentativa na grid principal, colorindo cada letra
    // 1. Marca verdes e registra letras usadas
    int usadas_palavra[TAMANHO_PALAVRA] = {0};
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
            aplicarCor(label, "#32CD32"); // Verde
            usadas_palavra[i] = 1;        // Marca como usada
        }
    }

    // 2. Marca amarelos e cinzas
    for (int i = 0; i < TAMANHO_PALAVRA; i++)
    {
        GtkWidget *label = gtk_grid_get_child_at(GTK_GRID(grid), i, tentativaAtual);
        if (tentativa[i] == palavraCerta[i])
            continue; // Já foi marcado como verde

        int achou = 0;
        for (int j = 0; j < TAMANHO_PALAVRA; j++)
        {
            if (!usadas_palavra[j] && tentativa[i] == palavraCerta[j])
            {
                achou = 1;
                usadas_palavra[j] = 1; // Marca como usada
                break;
            }
        }
        aplicarCor(label, achou ? "#FFD700" : "#696969"); // Amarelo ou cinza
    }

    gtk_widget_show_all(grid);
    tentativaAtual++;

    // <-- 3. ATUALIZAÇÃO DO LABEL A CADA TENTATIVA
    char texto_tentativas[50];
    int tentativas_restantes = MAX_TENTATIVAS - tentativaAtual;
    sprintf(texto_tentativas, "Tentativas restantes: %d", tentativas_restantes);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_tentativas);
    // Fim da atualização

    gtk_entry_set_text(GTK_ENTRY(entryPtr), "");

    // Verifica se o usuário acertou ou acabou as tentativas
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
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", mensagem);
        gtk_window_set_title(GTK_WINDOW(dialog), "Fim de Jogo");

        // Aplica CSS para fundo branco e texto escuro no diálogo
        GtkCssProvider *provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider,
            "* { background-color: #FFFFFF; color: #222222; }", -1, NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(dialog);
        gtk_style_context_add_provider(context,
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Fecha a janela do jogo e volta ao menu
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
        "Parabéns! Você acertou a palavra!");
    gtk_window_set_title(GTK_WINDOW(dialog), "Vitória!");

    // Aplica CSS para fundo branco e texto escuro no diálogo
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "* { background-color: #FFFFFF; color: #222222; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(dialog);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    // Fecha a janela do jogo e volta ao menu
    gtk_window_close(GTK_WINDOW(parent));
}

void iniciar_jogo_termo(int argc, char *argv[])
{
    tentativaAtual = 0; // Reinicia as tentativas

    gtk_init(&argc, &argv);

    // Carrega o dicionário e seleciona a palavra correta
    char palavras[MAX_WORDS][MAX_WORD_LENGTH];
    int numPalavras;
    carregarPalavras("dicionario.txt", palavras, &numPalavras);
    if (numPalavras == 0)
    {
        fprintf(stderr, "Nenhuma palavra válida encontrada no dicionario.txt!\n");
        exit(1);
    }
    selecionarPalavraAleatoria(palavras, numPalavras, palavraCerta);

    // Cria a janela principal
    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Termo - GTK");

    // Ajusta a altura da janela para caber 6 tentativas
    int altura_por_tentativa = 60;                                  // Altura aproximada para cada linha de tentativa
    int altura_total = altura_por_tentativa * MAX_TENTATIVAS + 200; // Aumentei para caber o novo label
    gtk_window_set_default_size(GTK_WINDOW(janela), 400, altura_total);

    // Centraliza a janela
    gtk_window_set_position(GTK_WINDOW(janela), GTK_WIN_POS_CENTER);


    // Define o estilo CSS para fundo, cores e tamanhos
   const char *css_data =
    "* { color: #222222; } "
    "label { color: #222222; font-weight: bold; font-size: 25px; padding: 10px; font-family: monospace; } "
    "entry { background-color: #FFFFFF; color: #222222; border: 3px solid #222222; font-size: 25px; font-family: monospace; } "
    "button { background-color: #EEEEEE; color: #222222; border: 3px solid #222222; font-size: 25px; font-family: monospace; }"
    "GtkWindow { background-color: #df1313; }";

GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(css_provider);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Cria a caixa vertical principal
    GtkWidget *caixaVertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(janela), caixaVertical);
    
    // <-- 2. CRIAÇÃO E POSICIONAMENTO DO LABEL
    labelTentativas = gtk_label_new(NULL);
    char texto_inicial[50];
    sprintf(texto_inicial, "Tentativas restantes: %d", MAX_TENTATIVAS);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_inicial);
    gtk_box_pack_start(GTK_BOX(caixaVertical), labelTentativas, FALSE, FALSE, 5);
    // Fim da criação


    // Cria a grid para as tentativas
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(caixaVertical), grid, TRUE, TRUE, 5);

    // Campo de entrada para o usuário digitar a palavra
    GtkWidget *entrada = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entrada), TAMANHO_PALAVRA);
    gtk_box_pack_start(GTK_BOX(caixaVertical), entrada, FALSE, FALSE, 5);

    // Botão para enviar a tentativa
    GtkWidget *botao = gtk_button_new_with_label("Enviar");
    gtk_box_pack_start(GTK_BOX(caixaVertical), botao, FALSE, FALSE, 5);

    // Inicializa o teclado virtual
    inicializarTeclado(caixaVertical);

    // Conecta os sinais dos botões e do campo de entrada
    g_signal_connect(botao, "clicked", G_CALLBACK(on_submit_clicked), entrada);
    g_signal_connect(entrada, "activate", G_CALLBACK(on_submit_clicked), entrada);

    gtk_widget_show_all(janela);
    gtk_main();
}