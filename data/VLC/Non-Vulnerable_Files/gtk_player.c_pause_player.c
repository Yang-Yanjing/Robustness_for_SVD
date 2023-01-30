}
void pause_player(void) {
    libvlc_media_player_pause(media_player);
    gtk_button_set_label(GTK_BUTTON(playpause_button), "gtk-media-play");
}
