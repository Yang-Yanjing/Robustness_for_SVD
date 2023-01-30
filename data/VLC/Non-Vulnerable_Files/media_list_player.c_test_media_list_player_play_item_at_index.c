}
static void test_media_list_player_play_item_at_index(const char** argv, int argc)
{
    libvlc_instance_t *vlc;
    libvlc_media_t *md;
    libvlc_media_list_t *ml;
    libvlc_media_list_player_t *mlp;
    const char * file = test_default_sample;
    log ("Testing play_item_at_index of %s using the media list.\n", file);
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    md = libvlc_media_new_path (vlc, file);
    assert(md);
    ml = libvlc_media_list_new (vlc);
    assert (ml != NULL);
    mlp = libvlc_media_list_player_new (vlc);
    assert(mlp);
    for (unsigned i = 0; i < 5; i++)
        libvlc_media_list_add_media( ml, md );
    libvlc_media_list_player_set_media_list( mlp, ml );
    libvlc_media_list_player_play_item_at_index( mlp, 0 );
    wait_playing (mlp);
    stop_and_wait (mlp);
    libvlc_media_release (md);
    libvlc_media_list_player_release (mlp);
    libvlc_release (vlc);
}
