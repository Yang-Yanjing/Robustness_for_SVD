}
void open_media(const char* uri) {
    libvlc_media_t *media;
    media = libvlc_media_new_location(vlc_inst, uri);
    libvlc_media_player_set_media(media_player, media);
    play();
    libvlc_media_release(media);
}
