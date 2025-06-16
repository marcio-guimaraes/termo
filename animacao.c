#include "animacao.h"

// Estrutura para manter os dados da nossa animação
typedef struct {
    GtkWidget *labels[5];
    int index_atual;
} AnimacaoData;

// Função auxiliar para aplicar cor e estilo a um label da animação
static void aplicarCorAnimacao(GtkWidget *widget, const char *corFundo, const char *corTexto) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();
    char css[256];
    snprintf(css, sizeof(css),
        "* { background-color: %s; color: %s; font-size: 40px; font-family: monospace; font-weight: bold; border-radius: 5px; padding: 5px; }",
        corFundo, corTexto);
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

// Função que será chamada pelo temporizador para revelar cada letra
static gboolean mostrar_letra_callback(gpointer data) {
    AnimacaoData *anim_data = (AnimacaoData *)data;

    if (anim_data->index_atual < 5) {
        // Torna a letra atual visível
        gtk_widget_show(anim_data->labels[anim_data->index_atual]);
        anim_data->index_atual++;
        return G_SOURCE_CONTINUE; // Manter o temporizador ativo
    }

    // Fim da animação
    g_free(anim_data); // Libera a memória
    return G_SOURCE_REMOVE; // Para o temporizador
}

// Função principal que configura e inicia a animação
void iniciar_animacao_termo(GtkWidget *container) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(grid, 20); // Espaço abaixo da animação

    // Adiciona o grid de animação no topo do container principal
    gtk_box_pack_start(GTK_BOX(container), grid, FALSE, FALSE, 0);

    const char *letras = "TERMO";
    const char *cores_fundo[] = {"#32CD32", "#FFD700", "#696969", "#FFD700", "#32CD32"}; // Verde, Amarelo, Cinza, Amarelo, Verde
    
    // Aloca memória para os dados da animação
    AnimacaoData *anim_data = g_new(AnimacaoData, 1);
    anim_data->index_atual = 0;

    for (int i = 0; i < 5; i++) {
        char str_letra[2] = {letras[i], '\0'};
        GtkWidget *label = gtk_label_new(str_letra);
        aplicarCorAnimacao(label, cores_fundo[i], "white");
        gtk_widget_set_hexpand(label, TRUE);
        
        // Esconde a letra inicialmente
        gtk_widget_set_no_show_all(label, TRUE);
        gtk_widget_hide(label);

        gtk_grid_attach(GTK_GRID(grid), label, i, 0, 1, 1);
        anim_data->labels[i] = label;
    }

    // Inicia um temporizador que chama a função de callback a cada 250ms
    g_timeout_add(250, mostrar_letra_callback, anim_data);
}