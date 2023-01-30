}
static void test_media_player_play_stop(const char** argv, int argc)
{
    libvlc_instance_t *vlc;
    libvlc_media_t *md;
    libvlc_media_player_t *mi;
    const char * file = test_default_sample;
    log ("Testing play and pause of %s\n", file);
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    md = libvlc_media_new_path (vlc, file);
    assert (md != NULL);
    mi = libvlc_media_player_new_from_media (md);
    assert (mi != NULL);
    libvlc_media_release (md);
    libvlc_media_player_play (mi);
    wait_playing (mi);
    libvlc_media_player_stop (mi);
    libvlc_media_player_release (mi);
    libvlc_release (vlc);
}
