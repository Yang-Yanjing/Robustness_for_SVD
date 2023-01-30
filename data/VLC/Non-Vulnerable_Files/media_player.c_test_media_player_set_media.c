}
static void test_media_player_set_media(const char** argv, int argc)
{
    const char * file = test_default_sample;
    log ("Testing set_media\n");
    libvlc_instance_t *vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    libvlc_media_t *md = libvlc_media_new_path (vlc, file);
    assert (md != NULL);
    libvlc_media_player_t *mp = libvlc_media_player_new (vlc);
    assert (mp != NULL);
    libvlc_media_player_set_media (mp, md);
    libvlc_media_release (md);
    libvlc_media_player_play (mp);
    wait_playing (mp);
    libvlc_media_player_stop (mp);
    libvlc_media_player_release (mp);
    libvlc_release (vlc);
}
