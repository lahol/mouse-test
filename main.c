#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>

GtkTextBuffer *text_buffer = NULL;
GtkAdjustment *vadjustment = NULL;

gboolean mt_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GString *msg = g_string_sized_new(256);
    GtkTextIter tend;

    g_string_printf(msg, "%" G_GUINT32_FORMAT ": ", event->time);
    switch (event->type) {
        case GDK_BUTTON_PRESS:
            g_string_append_printf(msg, "press ");
            break;
        case GDK_2BUTTON_PRESS:
            g_string_append_printf(msg, "2 button press ");
            break;
        case GDK_3BUTTON_PRESS:
            g_string_append_printf(msg, "3 button press ");
            break;
        case GDK_BUTTON_RELEASE:
            g_string_append_printf(msg, "release ");
            break;
        default:
            g_string_append_printf(msg, "unhandled event ");
    }

    if (event->button == 1)
        g_string_append_printf(msg, "LMB\n");
    else if (event->button == 2)
        g_string_append_printf(msg, "MMB\n");
    else if (event->button == 3)
        g_string_append_printf(msg, "RMB\n");
    else
        g_string_append_printf(msg, "%u\n", event->button);

    gchar *str = g_string_free(msg, FALSE);

    gtk_text_buffer_get_iter_at_offset(text_buffer, &tend, -1);
    gtk_text_buffer_insert(text_buffer, &tend, str, -1);

    fputs(str, stdout);
    fflush(stdout);
    g_free(str);

    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment)); 

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

    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *frame = gtk_frame_new("Click area");
    gtk_container_add(GTK_CONTAINER(frame), event_box);

    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scrolled_win = gtk_scrolled_window_new(NULL, NULL);
    vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_win));
    gtk_container_add(GTK_CONTAINER(scrolled_win), text_view);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkWidget *vbox = gtk_vbox_new(TRUE, 3);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_win, TRUE, TRUE, 2);
    gtk_container_add(GTK_CONTAINER(win), vbox);
    gtk_widget_show_all(vbox);

    gtk_window_set_default_size(GTK_WINDOW(win), 400, 300);
    gtk_widget_add_events(GTK_WIDGET(event_box),
            GDK_BUTTON_PRESS_MASK |
            GDK_BUTTON_RELEASE_MASK |
            GDK_KEY_RELEASE_MASK |
            GDK_SCROLL_MASK);

    g_signal_connect(G_OBJECT(event_box), "button-press-event",
            G_CALLBACK(mt_window_button_event), NULL);
    g_signal_connect(G_OBJECT(event_box), "button-release-event",
            G_CALLBACK(mt_window_button_event), NULL);
    g_signal_connect(G_OBJECT(win), "key-release-event",
            G_CALLBACK(mt_window_key_release_event), NULL);

    gtk_window_present(GTK_WINDOW(win));

    gtk_main();

    return 0;
}
