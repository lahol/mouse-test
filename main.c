#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>

gboolean mt_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    fprintf(stdout, "%" G_GUINT32_FORMAT " button event: ", event->time);
    switch (event->type) {
        case GDK_BUTTON_PRESS:
            fprintf(stdout, "press ");
            break;
        case GDK_2BUTTON_PRESS:
            fprintf(stdout, "2 button press ");
            break;
        case GDK_3BUTTON_PRESS:
            fprintf(stdout, "3 button press ");
            break;
        case GDK_BUTTON_RELEASE:
            fprintf(stdout, "release ");
            break;
        default:
            fprintf(stdout, "unhandled event ");
    }

    fprintf(stdout, "%u\n", event->button);

    return TRUE;
}

#ifndef GDK_KEY_Escape
#define GDK_KEY_Escape 0xff1b
#endif

gboolean mt_window_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_Escape)
        gtk_main_quit();
    return TRUE;
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (win == NULL)
        return 1;

    gtk_window_set_default_size(GTK_WINDOW(win), 400, 300);
    gtk_widget_add_events(GTK_WIDGET(win),
            GDK_BUTTON_PRESS_MASK |
            GDK_BUTTON_RELEASE_MASK |
            GDK_KEY_RELEASE_MASK |
            GDK_SCROLL_MASK);

    g_signal_connect(G_OBJECT(win), "button-press-event",
            G_CALLBACK(mt_window_button_event), NULL);
    g_signal_connect(G_OBJECT(win), "button-release-event",
            G_CALLBACK(mt_window_button_event), NULL);
    g_signal_connect(G_OBJECT(win), "key-release-event",
            G_CALLBACK(mt_window_key_release_event), NULL);

    gtk_window_present(GTK_WINDOW(win));

    gtk_main();

    return 0;
}
