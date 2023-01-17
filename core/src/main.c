#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

static GtkListStore *grep_poppulate()
{
    GtkTreeIter iter;
    FILE *f;
    char buf[64];
    GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);
    f = popen("ls /bin/ -p | grep -v /", "r");
    while (fgets(buf, 64, f) != NULL)
    {
        for (char i = 0; i < 64; i++)
            if (buf[i] == '\n')
                buf[i] = 0;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, buf, -1);
    }
    return store;
}

static gboolean key_press(GtkWidget *window, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_Escape)
        exit(0);
    return FALSE;
}

static void entry_activate(GtkEntry *entry, gpointer data)
{
    char c[256] = "/bin/sh -c ";
    strcat(c, gtk_entry_get_text(entry));
    system(c);
    exit(0);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "atorm");
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

    g_signal_connect(window, "key_press_event", G_CALLBACK(key_press), NULL);

    GtkWidget *entry = gtk_entry_new();
    GtkWidget *comp = gtk_entry_completion_new();
    GtkEntryBuffer *buff = gtk_entry_buffer_new("", 0);
    GtkTreeIter iter;
    GtkListStore *store = grep_poppulate();
    gtk_entry_set_completion(GTK_ENTRY(entry), GTK_ENTRY_COMPLETION(comp));
    gtk_entry_completion_set_inline_completion(GTK_ENTRY_COMPLETION(comp), TRUE);
    gtk_entry_completion_set_inline_selection(GTK_ENTRY_COMPLETION(comp), TRUE);
    gtk_entry_completion_set_popup_completion(GTK_ENTRY_COMPLETION(comp), TRUE);
    gtk_entry_completion_set_popup_single_match(GTK_ENTRY_COMPLETION(comp), FALSE);
    gtk_entry_completion_set_model(GTK_ENTRY_COMPLETION(comp), GTK_TREE_MODEL(store));
    gtk_entry_completion_set_text_column(GTK_ENTRY_COMPLETION(comp), 0);
    gtk_entry_set_buffer(GTK_ENTRY(entry), buff);

    g_signal_connect(entry, "activate", G_CALLBACK(entry_activate), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(box), entry);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all(window);

    int x, y, h, w;
    gtk_widget_get_pointer(window, &x, &y);
    gtk_window_get_size(GTK_WINDOW(window), &w, &h);
    gtk_window_move(GTK_WINDOW(window), x - w / 2, y - h / 2);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("universai.atorm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}