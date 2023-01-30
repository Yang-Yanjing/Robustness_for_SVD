}
void play(void) {
    libvlc_media_player_play(media_player);
    gtk_button_set_label(GTK_BUTTON(playpause_button), "gtk-media-pause");
}
