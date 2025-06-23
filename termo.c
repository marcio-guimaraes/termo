#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "jogo_utils.h" // Inclui as funções utilitárias refatoradas

// --- Constantes do Jogo ---
#define MAX_TENTATIVAS 6
// TAMANHO_PALAVRA e MAX_WORD_LENGTH agora vêm de jogo_utils.h
#define MAX_WORDS_SORTEIO 10000
#define MAX_WORDS_VALIDACAO 30000

// --- Variáveis Globais ---
char palavraCerta[MAX_WORD_LENGTH]; // Esta vai armazenar a palavra sorteada ORIGINAL (com acentos)
char palavraCerta_normalizada[MAX_WORD_LENGTH]; // Esta vai armazenar a palavra sorteada NORMALIZADA (sem acentos)
GtkWidget *grid;
GtkWidget *tecladoGrid;
GtkWidget *teclas[26];
GtkWidget *labelTentativas;
GtkWidget *labelMensagem;
GtkWidget *grid_tiles[MAX_TENTATIVAS][TAMANHO_PALAVRA];
int mapeamentoTeclas[26];
int tentativaAtual = 0;
char palavras_sorteio[MAX_WORDS_SORTEIO][MAX_WORD_LENGTH];
int numPalavras_sorteio;
char palavras_existentes[MAX_WORDS_VALIDACAO][MAX_WORD_LENGTH];
int numPalavras_existentes;

// --- Protótipos de Funções ---
void mostrarTelaVitoria(GtkWidget *parent);
void iniciar_jogo_termo(int argc, char *argv[]);

// --- FUNÇÕES AUXILIARES (Específicas do Jogo) ---

// --- Função auxiliar: limpa mensagem de erro após timeout ---
gboolean limpar_label_mensagem(gpointer data) {
    gtk_label_set_text(GTK_LABEL(data), "");
    return G_SOURCE_REMOVE;
}

// --- Função auxiliar: valida se a palavra tentada existe no dicionário ---
const char* validar_e_corrigir_palavra(const char *tentativa_normalizada_para_busca) {
    char palavra_dicionario_normalizada_para_busca[MAX_WORD_LENGTH];
    for (int i = 0; i < numPalavras_existentes; i++) {
        // Normaliza a palavra do dicionário para a busca (sem acentos, maiúsculas)
        normalizar_palavra(palavras_existentes[i], palavra_dicionario_normalizada_para_busca);
        if (strcmp(tentativa_normalizada_para_busca, palavra_dicionario_normalizada_para_busca) == 0) {
            return palavras_existentes[i]; // Retorna a palavra ORIGINAL do dicionário
        }
    }
    return NULL; // Não encontrou
}

// --- Inicializa o teclado virtual ---
void inicializarTeclado(GtkWidget *caixaVertical) {
    tecladoGrid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(tecladoGrid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(tecladoGrid), 5);
    gtk_box_pack_end(GTK_BOX(caixaVertical), tecladoGrid, FALSE, FALSE, 5);
    const char *layout = "QWERTYUIOPASDFGHJKLZXCVBNM";
    int linha = 0, coluna = 0;
    for (int i = 0; i < 26; i++) {
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
        if ((i == 9) || (i == 18)) {
            linha++;
            coluna = 0;
        }
    }
}

// --- Atualiza as cores do teclado após tentativa ---
void atualizarTeclado(const char *palavra_tentada_original, const char *palavra_certa_original) {
    // Normaliza ambas as palavras para maiúsculas e sem acentos para a lógica de coloração do teclado.
    char tentada_normalizada_teclado[MAX_WORD_LENGTH];
    char certa_normalizada_teclado[MAX_WORD_LENGTH];
    normalizar_palavra(palavra_tentada_original, tentada_normalizada_teclado);
    normalizar_palavra(palavra_certa_original, certa_normalizada_teclado);

    int num_chars = g_utf8_strlen(tentada_normalizada_teclado, -1);
    if (num_chars == -1) return; // Erro ou string vazia

    int letras_certa_usadas_teclado[TAMANHO_PALAVRA] = {0}; // Marca letras da palavra certa que já foram "usadas"

    // Letras corretas (verde) para o teclado
    for (int i = 0; i < num_chars; i++) {
        gunichar uc_tentada = g_utf8_get_char(g_utf8_offset_to_pointer(tentada_normalizada_teclado, i));
        gunichar uc_certa = g_utf8_get_char(g_utf8_offset_to_pointer(certa_normalizada_teclado, i));

        if (uc_tentada == uc_certa) {
            if (uc_tentada >= 'A' && uc_tentada <= 'Z') { // Mapeia apenas letras A-Z
                int indice = mapeamentoTeclas[uc_tentada - 'A'];
                if (indice >= 0 && indice < 26) aplicar_cor_widget(teclas[indice], "#32CD32");
            }
            letras_certa_usadas_teclado[i] = 1;
        }
    }

    // Letras existentes em posição errada (amarelo) ou inexistentes (cinza) para o teclado
    for (int i = 0; i < num_chars; i++) {
        gunichar uc_tentada = g_utf8_get_char(g_utf8_offset_to_pointer(tentada_normalizada_teclado, i));
        gunichar uc_certa_i = g_utf8_get_char(g_utf8_offset_to_pointer(certa_normalizada_teclado, i));

        if (uc_tentada == uc_certa_i) continue; // Se já foi marcada como verde, pular

        int achou_na_palavra_certa = 0;
        for (int j = 0; j < num_chars; j++) {
            gunichar uc_certa_j = g_utf8_get_char(g_utf8_offset_to_pointer(certa_normalizada_teclado, j));
            if (!letras_certa_usadas_teclado[j] && uc_tentada == uc_certa_j) {
                achou_na_palavra_certa = 1;
                letras_certa_usadas_teclado[j] = 1; // Marcar como usada
                break;
            }
        }

        if (uc_tentada >= 'A' && uc_tentada <= 'Z') {
            int indice = mapeamentoTeclas[uc_tentada - 'A'];
            if (indice >= 0 && indice < 26) {
                aplicar_cor_widget(teclas[indice], achou_na_palavra_certa ? "#FFD700" : "#696969");
            }
        }
    }
}

// --- Lógica de processamento ao enviar tentativa ---
void on_submit_clicked(GtkButton *botao, gpointer entryPtr) {
    if (tentativaAtual >= MAX_TENTATIVAS) return;

    const char *entrada_original = gtk_entry_get_text(GTK_ENTRY(entryPtr));
    int len_entrada = g_utf8_strlen(entrada_original, -1);
    if (len_entrada == -1 || len_entrada != TAMANHO_PALAVRA) {
        gtk_label_set_text(GTK_LABEL(labelMensagem), "Palavra deve ter 5 letras!");
        g_timeout_add(2000, limpar_label_mensagem, labelMensagem);
        gtk_entry_set_text(GTK_ENTRY(entryPtr), "");
        return;
    }

    // A palavra digitada pelo usuário, normalizada (sem acentos, maiúsculas), para busca no dicionário
    char tentativa_para_busca[MAX_WORD_LENGTH];
    normalizar_palavra(entrada_original, tentativa_para_busca);

    // Valida se a palavra (normalizada para busca) existe no dicionário
    // Retorna a palavra ORIGINAL (com acentos) do dicionário se encontrada.
    const char *palavra_corrigida_do_dicionario = validar_e_corrigir_palavra(tentativa_para_busca);

    if (palavra_corrigida_do_dicionario == NULL) {
        gtk_label_set_text(GTK_LABEL(labelMensagem), "Palavra inválida!");
        g_timeout_add(2000, limpar_label_mensagem, labelMensagem);
        gtk_entry_set_text(GTK_ENTRY(entryPtr), "");
        return;
    }

    // Atualiza o teclado usando a palavra ORIGINAL do dicionário e a palavra secreta ORIGINAL
    // (A função atualizarTeclado internamente as normaliza para as comparações das teclas)
    atualizarTeclado(palavra_corrigida_do_dicionario, palavraCerta);

    // --- Lógica para colorir os tiles do grid ---
    // Agora, as comparações para colorir os tiles IGNORAM ACENTOS, usando as versões NORMALIZADAS das palavras.
    char tentada_normalizada_para_tiles[MAX_WORD_LENGTH];
    char certa_normalizada_para_tiles[MAX_WORD_LENGTH];
    normalizar_palavra(palavra_corrigida_do_dicionario, tentada_normalizada_para_tiles); // Normaliza a palavra do dicionário
    normalizar_palavra(palavraCerta, certa_normalizada_para_tiles);                   // Normaliza a palavra certa

    const gchar *p_tentada_para_tiles = tentada_normalizada_para_tiles;
    const gchar *p_certa_para_tiles = certa_normalizada_para_tiles;

    int num_chars_tentada_norm = g_utf8_strlen(p_tentada_para_tiles, -1);
    // if (num_chars_tentada_norm == -1) já verificado acima.

    int usadas_palavra_certa_para_tiles[TAMANHO_PALAVRA] = {0}; // Marca letras da palavra certa (normalizada) que já foram usadas

    // Preenche os labels do grid com a palavra tentada ORIGINAL (com acento, se corrigida pelo dicionário)
    const gchar *p_entrada_original_para_label = palavra_corrigida_do_dicionario;
    for(int i = 0; i < num_chars_tentada_norm; i++) { // Usa num_chars_tentada_norm para iterar pelos caracteres
        GtkWidget *label = grid_tiles[tentativaAtual][i];
        gunichar uc_char = g_utf8_get_char(g_utf8_offset_to_pointer(p_entrada_original_para_label, i));
        char char_str[8] = {0}; // Buffer para o caractere UTF-8
        g_unichar_to_utf8(uc_char, char_str); // Converte o gunichar de volta para UTF-8 para o label
        gtk_label_set_text(GTK_LABEL(label), char_str);
    }

    // Letras corretas na posição certa (verde)
    for (int i = 0; i < num_chars_tentada_norm; i++) {
        gunichar uc_tentada_norm = g_utf8_get_char(g_utf8_offset_to_pointer(p_tentada_para_tiles, i));
        gunichar uc_certa_norm = g_utf8_get_char(g_utf8_offset_to_pointer(p_certa_para_tiles, i));

        if (uc_tentada_norm == uc_certa_norm) {
            aplicar_cor_widget(grid_tiles[tentativaAtual][i], "#32CD32");
            usadas_palavra_certa_para_tiles[i] = 1; // Marca a letra na palavra certa normalizada como usada
        }
    }

    // Letras presentes, mas na posição errada (amarelo) ou inexistentes (cinza)
    for (int i = 0; i < num_chars_tentada_norm; i++) {
        gunichar uc_tentada_norm = g_utf8_get_char(g_utf8_offset_to_pointer(p_tentada_para_tiles, i));
        gunichar uc_certa_na_pos_atual_norm = g_utf8_get_char(g_utf8_offset_to_pointer(p_certa_para_tiles, i));

        // Se a letra já foi marcada como verde na iteração anterior, pula
        if (uc_tentada_norm == uc_certa_na_pos_atual_norm) continue;

        int achou_em_outra_posicao = 0;
        for (int j = 0; j < num_chars_tentada_norm; j++) {
            gunichar uc_certa_em_j_norm = g_utf8_get_char(g_utf8_offset_to_pointer(p_certa_para_tiles, j));
            // Se a letra da palavra certa (normalizada) na posição 'j' ainda não foi usada e corresponde à letra tentada (normalizada)
            if (!usadas_palavra_certa_para_tiles[j] && uc_tentada_norm == uc_certa_em_j_norm) {
                achou_em_outra_posicao = 1;
                usadas_palavra_certa_para_tiles[j] = 1; // Marca como usada
                break;
            }
        }
        aplicar_cor_widget(grid_tiles[tentativaAtual][i], achou_em_outra_posicao ? "#FFD700" : "#696969");
    }

    tentativaAtual++;
    
    char texto_tentativas[50];
    int tentativas_restantes = MAX_TENTATIVAS - tentativaAtual;
    snprintf(texto_tentativas, sizeof(texto_tentativas), "Tentativas restantes: %d", tentativas_restantes);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_tentativas);

    gtk_entry_set_text(GTK_ENTRY(entryPtr), "");

    // A comparação final agora compara a palavra do dicionário (que pode ser com acento)
    // com a palavra secreta original (com acento).
    // Se a intenção é que "AVIAO" e "AVIÃO" sejam considerados o mesmo para VITÓRIA,
    // aqui você deveria comparar a tentativa normalizada com a palavraCerta_normalizada.
    // Por exemplo: if (strcmp(tentativa_para_busca, palavraCerta_normalizada) == 0)
    if (strcmp(palavra_corrigida_do_dicionario, palavraCerta) == 0) {
        mostrarTelaVitoria(gtk_widget_get_toplevel(GTK_WIDGET(botao)));
    } else if (tentativaAtual == MAX_TENTATIVAS) {
        char mensagem[256];
        snprintf(mensagem, sizeof(mensagem), "A palavra era: %s", palavraCerta);
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(botao))), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", mensagem);
        gtk_window_set_title(GTK_WINDOW(dialog), "Fim de Jogo");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_window_close(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(botao))));
    }
}

// --- Exibe tela de vitória ---
void mostrarTelaVitoria(GtkWidget *parent) {
    char mensagem[256];
    snprintf(mensagem, sizeof(mensagem), "Parabéns, você acertou!\n\nA palavra era: %s", palavraCerta);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", mensagem);
    gtk_window_set_title(GTK_WINDOW(dialog), "Vitória!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gtk_window_close(GTK_WINDOW(parent));
}

// --- Função principal do jogo ---
void iniciar_jogo_termo(int argc, char *argv[]) {
    tentativaAtual = 0;
    gtk_init(&argc, &argv);

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Carregamento de dicionário de palavras usando a função utilitária
    numPalavras_sorteio = carregar_palavras_do_arquivo("palavras.txt", palavras_sorteio, MAX_WORDS_SORTEIO);
    if (numPalavras_sorteio == 0) {
        fprintf(stderr, "Nenhuma palavra de sorteio carregada. Verifique o arquivo palavras.txt\n");
        exit(1);
    }
    
    numPalavras_existentes = carregar_palavras_do_arquivo("palavras.txt", palavras_existentes, MAX_WORDS_VALIDACAO);
    if (numPalavras_existentes == 0) {
        fprintf(stderr, "Nenhuma palavra de validação carregada. Verifique o arquivo palavras.txt\n");
        exit(1);
    }

    // Seleciona a palavra do jogo e a armazena na sua forma original
    selecionar_palavra_aleatoria(palavras_sorteio, numPalavras_sorteio, palavraCerta);
    // Também armazena a versão normalizada (sem acentos) para comparações de coloração e vitória
    normalizar_palavra(palavraCerta, palavraCerta_normalizada);


    // Construção da interface GTK
    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Termo - Desktop");
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
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout principal
    GtkWidget *caixaVertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(janela), caixaVertical);

    // Label de tentativas
    labelTentativas = gtk_label_new(NULL);
    gtk_widget_set_name(labelTentativas, "tentativas");
    char texto_inicial[50];
    snprintf(texto_inicial, sizeof(texto_inicial), "Tentativas restantes: %d", MAX_TENTATIVAS);
    gtk_label_set_text(GTK_LABEL(labelTentativas), texto_inicial);
    gtk_box_pack_start(GTK_BOX(caixaVertical), labelTentativas, FALSE, FALSE, 5);
    
    // Label de mensagens de erro
    labelMensagem = gtk_label_new("");
    gtk_widget_set_name(labelMensagem, "mensagem");
    gtk_box_pack_start(GTK_BOX(caixaVertical), labelMensagem, FALSE, FALSE, 5);

    // Grid das letras
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(caixaVertical), grid, TRUE, TRUE, 5);

    // Cria os tiles (labels) de cada tentativa/letra
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

    // Campo de entrada
    GtkWidget *entrada = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entrada), 0);
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