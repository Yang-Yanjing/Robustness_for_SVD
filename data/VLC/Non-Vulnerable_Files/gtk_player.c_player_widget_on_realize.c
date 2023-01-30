}
void player_widget_on_realize(GtkWidget *widget, gpointer data) {
    libvlc_media_player_set_xwindow(media_player, GDK_WINDOW_XID(gtk_widget_get_window(widget)));
}
