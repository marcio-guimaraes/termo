#include <gtk/gtk.h>
#include "animacao.h"

// Declara a função do termo.c para ser usada aqui
void iniciar_jogo_termo(int argc, char *argv[]);

// Função para aplicar o estilo CSS global à aplicação
void aplicarEstiloGlobal() {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css_data =
        "* { color: #222222; } "
        "label { color: #222222; font-weight: bold; font-size: 25px; padding: 10px; } "
        "entry { background-color: #FFFFFF; color: #222222; border: 3px solid #222222; font-size: 25px; } "
        "button { background-color: #EEEEEE; color: #222222; border: 3px solid #222222; font-size: 25px; }"
        "GtkWindow { background-color: #df1313; }";
    
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

// Funções de callback para os botões do menu
void on_jogar_clicked(GtkButton *button, gpointer user_data) {
    gtk_widget_hide(GTK_WIDGET(user_data)); // Esconde a janela do menu
    iniciar_jogo_termo(0, NULL);            // Inicia a lógica do jogo
    gtk_widget_show(GTK_WIDGET(user_data)); // Mostra o menu novamente quando o jogo termina
}

void on_descricao_clicked(GtkButton *button, gpointer user_data) {
    // Usamos 'gtk_message_dialog_new_with_markup' para permitir a formatação do texto.
    GtkWidget *dialog = gtk_message_dialog_new_with_markup(
        GTK_WINDOW(user_data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Descubra a palavra secreta em até 6 tentativas!");

    // Montamos o texto secundário com a formatação Pango.
    // As cores são as mesmas usadas no jogo termo
    const char *secondary_text =
        "\n<b>Regras:</b>\n"
        "Após cada tentativa, as letras mudam de cor:\n\n"
        "   <span weight='bold' font_family='monospace' background='#32CD32' foreground='white'> C </span>  A letra está no lugar certo.\n\n"
        "   <span weight='bold' font_family='monospace' background='#FFD700' foreground='white'> A </span>  A letra existe na palavra, mas no lugar errado.\n\n"
        "   <span weight='bold' font_family='monospace' background='#696969' foreground='white'> S </span>  A letra não existe na palavra.\n\n"
        "<b>Exemplo:</b>\n"
        "Palavra secreta: 'PORTA', tentativa: 'TORRE'\n\n"
        "   <span weight='bold' font_family='monospace' background='#FFD700' foreground='white'> T </span> "
        "<span weight='bold' font_family='monospace' background='#32CD32' foreground='white'> O </span> "
        "<span weight='bold' font_family='monospace' background='#32CD32' foreground='white'> R </span> "
        "<span weight='bold' font_family='monospace' background='#696969' foreground='white'> R </span> "
        "<span weight='bold' font_family='monospace' background='#696969' foreground='white'> E </span> "
        "\n\nBoa sorte!";

    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), "%s", secondary_text);
    gtk_window_set_title(GTK_WINDOW(dialog), "Descrição do Jogo");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_creditos_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(user_data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "\nDesenvolvido por Márcio Guimarães");
    gtk_window_set_title(GTK_WINDOW(dialog), "Créditos");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_sair_clicked(GtkButton *button, gpointer user_data) {
    gtk_window_close(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))));
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Aplica o estilo global antes de criar qualquer janela
    aplicarEstiloGlobal();

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Menu");
    gtk_window_set_default_size(GTK_WINDOW(janela), 300, 400);
    gtk_window_set_position(GTK_WINDOW(janela), GTK_WIN_POS_CENTER);

    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(caixa), 30);

    iniciar_animacao_termo(caixa);

    GtkWidget *btn_jogar = gtk_button_new_with_label("Jogar");
    GtkWidget *btn_descricao = gtk_button_new_with_label("Descrição");
    GtkWidget *btn_creditos = gtk_button_new_with_label("Créditos");
    GtkWidget *btn_sair = gtk_button_new_with_label("Sair");

    gtk_box_pack_start(GTK_BOX(caixa), btn_jogar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_descricao, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_creditos, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_sair, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(janela), caixa);

    // Conecta os sinais dos botões às suas respectivas funções
    g_signal_connect(btn_jogar, "clicked", G_CALLBACK(on_jogar_clicked), janela);
    g_signal_connect(btn_descricao, "clicked", G_CALLBACK(on_descricao_clicked), janela);
    g_signal_connect(btn_creditos, "clicked", G_CALLBACK(on_creditos_clicked), janela);
    g_signal_connect(btn_sair, "clicked", G_CALLBACK(on_sair_clicked), janela);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(janela);
    gtk_main();
    return 0;
}