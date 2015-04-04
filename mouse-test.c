/*
 * mouse-test -- monitor mouse click events
 * Copyright (c) 2015 Holger Langenau (see also: LICENSE)
 * 
 */
#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>

GtkTextBuffer *text_buffer = NULL;
GtkAdjustment *vadjustment = NULL;

struct {
    gboolean insert_empty_line;
    guint32 idle_interval;
    guint source_id;
} mt_idle_status;

gboolean idle_proc(gpointer data)
{
    GtkTextIter tend;
    gtk_text_buffer_get_iter_at_offset(text_buffer, &tend, -1);
    gtk_text_buffer_insert(text_buffer, &tend, "–\n", -1);

    mt_idle_status.source_id = 0;
    return FALSE;
}

void mt_start_idle(void)
{
    if (mt_idle_status.source_id > 0) {
        g_source_remove(mt_idle_status.source_id);
    }
    if (mt_idle_status.insert_empty_line) {
        mt_idle_status.source_id = g_timeout_add(mt_idle_status.idle_interval, idle_proc, NULL);
    }
}

void mt_idle_interval_changed(GtkSpinButton *spin_button, gpointer data)
{
    mt_idle_status.idle_interval = (guint32)gtk_spin_button_get_value_as_int(spin_button);
}

void mt_idle_toggle_insert_empty_line(GtkToggleButton *toggle_button, gpointer data)
{
    mt_idle_status.insert_empty_line = gtk_toggle_button_get_active(toggle_button);
}

gboolean mt_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GString *msg = g_string_sized_new(256);
    GtkTextIter tend;
/*    static guint32 last_event = 0;

    if (last_event > 0 && event->time - last_event >= 1000)
        g_string_append_c(msg, '\n');
    last_event = event->time;*/

    g_string_append_printf(msg, "%" G_GUINT32_FORMAT ": ", event->time);
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
    g_free(str);

    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));

    mt_start_idle();

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

    mt_idle_status.insert_empty_line = TRUE;
    mt_idle_status.idle_interval = 800;

    GtkWidget *vbox = gtk_vbox_new(FALSE, 3);

    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *frame = gtk_frame_new("Click area");
    gtk_widget_set_size_request(frame, 400, 200);
    gtk_container_add(GTK_CONTAINER(frame), event_box);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 2);

    GtkWidget *hbox = gtk_hbox_new(FALSE, 3);
    GtkWidget *check_box = gtk_check_button_new_with_label("Insert empty line after milliseconds:");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_box), mt_idle_status.insert_empty_line);
    g_signal_connect(G_OBJECT(check_box), "toggled", G_CALLBACK(mt_idle_toggle_insert_empty_line), NULL);

    gtk_box_pack_start(GTK_BOX(hbox), check_box, FALSE, FALSE, 2);

    GtkWidget *spin_button = gtk_spin_button_new_with_range(0, 10000, 100);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), mt_idle_status.idle_interval);
    g_signal_connect(G_OBJECT(spin_button), "value-changed", G_CALLBACK(mt_idle_interval_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), spin_button, FALSE, FALSE, 2);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

    GtkWidget *text_view = gtk_text_view_new();
    GtkWidget *scrolled_win = gtk_scrolled_window_new(NULL, NULL);
    vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_win));
    gtk_container_add(GTK_CONTAINER(scrolled_win), text_view);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkWidget *vpane = gtk_vpaned_new();
    gtk_paned_pack1(GTK_PANED(vpane), vbox, FALSE, FALSE);
    gtk_paned_pack2(GTK_PANED(vpane), scrolled_win, TRUE, FALSE);
    gtk_container_add(GTK_CONTAINER(win), vpane);
    gtk_widget_show_all(vpane);

    gtk_window_set_default_size(GTK_WINDOW(win), 400, 600);
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
