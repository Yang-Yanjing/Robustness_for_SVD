}
static void test_media_list_player_previous(const char** argv, int argc)
{
    libvlc_instance_t *vlc;
    libvlc_media_t *md;
    libvlc_media_list_t *ml;
    libvlc_media_list_player_t *mlp;
    const char * file = test_default_sample;
    log ("Testing media player previous()\n");
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    md = libvlc_media_new_path (vlc, file);
    assert(md);
    ml = libvlc_media_list_new (vlc);
    assert (ml != NULL);
    mlp = libvlc_media_list_player_new (vlc);
    assert(mlp);;
    libvlc_media_list_add_media (ml, md);
    // Add three media
    media_list_add_file_path (vlc, ml, file);
    media_list_add_file_path (vlc, ml, file);
    media_list_add_file_path (vlc, ml, file);
    libvlc_media_list_player_set_media_list (mlp, ml);
    libvlc_media_list_player_play_item (mlp, md);
    wait_playing (mlp);
    libvlc_media_release (md);
    libvlc_media_list_player_previous (mlp);
    wait_playing (mlp);
    libvlc_media_list_player_pause (mlp);
    libvlc_media_list_player_previous (mlp);
    wait_playing (mlp);
    stop_and_wait (mlp);
    libvlc_media_list_player_previous (mlp);
    wait_playing (mlp);
    stop_and_wait (mlp);
    libvlc_media_list_player_release (mlp);
    libvlc_release (vlc);
}
