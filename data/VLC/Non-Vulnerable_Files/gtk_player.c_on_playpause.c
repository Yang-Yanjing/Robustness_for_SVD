}
void on_playpause(GtkWidget *widget, gpointer data) {
    if(libvlc_media_player_is_playing(media_player) == 1) {
        pause_player();
    }
    else {
        play();
    }
}
}
void on_playpause(GtkWidget *widget, gpointer data) {
    if(libvlc_media_player_is_playing(media_player) == 1) {
        pause_player();
    }
    else {
        play();
    }
}
