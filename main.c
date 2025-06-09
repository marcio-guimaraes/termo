#include <gtk/gtk.h>

// Declare a função do termo.c
void iniciar_jogo_termo(int argc, char *argv[]);

// Funções de callback para os botões
void on_jogar_clicked(GtkButton *button, gpointer user_data)
{
    gtk_widget_hide(GTK_WIDGET(user_data)); // Esconde o menu
    iniciar_jogo_termo(0, NULL);            // Inicia o jogo
    gtk_widget_show(GTK_WIDGET(user_data)); // Mostra o menu ao sair do jogo
}

void on_descricao_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(user_data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Descubra a palavra secreta em até 6 tentativas!\n"
        "\n"
        "Regras:\n"
        "- Digite uma palavra de 5 letras por tentativa.\n"
        "- Só são aceitas palavras válidas do dicionário.\n"
        "- Após cada tentativa, as letras mudam de cor:\n"
        "   • Verde: a letra está na posição correta.\n"
        "   • Amarelo: a letra existe na palavra, mas em outra posição.\n"
        "   • Cinza: a letra não existe na palavra.\n"
        "- Use as dicas de cores para adivinhar a palavra certa!\n"
        "\n"
        "Boa sorte!");
    gtk_window_set_title(GTK_WINDOW(dialog), "Descrição do Jogo");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_creditos_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(user_data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Termo GTK\nDesenvolvido por Márcio Guimarães");
    gtk_window_set_title(GTK_WINDOW(dialog), "Créditos");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_sair_clicked(GtkButton *button, gpointer user_data)
{
    gtk_window_close(GTK_WINDOW(user_data));
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Menu - Termo");
    gtk_window_set_default_size(GTK_WINDOW(janela), 300, 400);
    gtk_window_set_position(GTK_WINDOW(janela), GTK_WIN_POS_CENTER);

    GtkWidget *caixa = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(caixa), 30);

    GtkWidget *btn_jogar = gtk_button_new_with_label("Jogar");
    GtkWidget *btn_descricao = gtk_button_new_with_label("Descrição");
    GtkWidget *btn_creditos = gtk_button_new_with_label("Créditos");
    GtkWidget *btn_sair = gtk_button_new_with_label("Sair");

    gtk_box_pack_start(GTK_BOX(caixa), btn_jogar, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_descricao, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_creditos, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(caixa), btn_sair, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(janela), caixa);

    // Conecta os sinais dos botões
    g_signal_connect(btn_jogar, "clicked", G_CALLBACK(on_jogar_clicked), janela);
    g_signal_connect(btn_descricao, "clicked", G_CALLBACK(on_descricao_clicked), janela);
    g_signal_connect(btn_creditos, "clicked", G_CALLBACK(on_creditos_clicked), janela);
    g_signal_connect(btn_sair, "clicked", G_CALLBACK(on_sair_clicked), janela);

    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(janela);
    gtk_main();

    return 0;
}