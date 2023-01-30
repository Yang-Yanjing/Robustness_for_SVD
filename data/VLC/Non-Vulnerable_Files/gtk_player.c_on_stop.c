}
void on_stop(GtkWidget *widget, gpointer data) {
    pause_player();
    libvlc_media_player_stop(media_player);
}
