}
static void test_media_list_player_items_queue(const char** argv, int argc)
{
    libvlc_instance_t *vlc;
    libvlc_media_t *md;
    libvlc_media_list_t *ml;
    libvlc_media_list_player_t *mlp;
    const char * file = test_default_sample;
    log ("Testing media player item queue-ing\n");
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    md = libvlc_media_new_path (vlc, file);
    assert(md);
    ml = libvlc_media_list_new (vlc);
    assert (ml != NULL);
    mlp = libvlc_media_list_player_new (vlc);
    assert(mlp);
    libvlc_media_list_add_media (ml, md);
    static struct check_items_order_data check;
    check_data_init(&check);
    queue_expected_item(&check, md);
    // Add three more media
    queue_expected_item(&check, media_list_add_file_path (vlc, ml, file));
    queue_expected_item(&check, media_list_add_file_path (vlc, ml, file));
    queue_expected_item(&check, media_list_add_file_path (vlc, ml, file));
    // Add a node
    libvlc_media_t *node = libvlc_media_new_as_node(vlc, "node");
    assert(node);
    libvlc_media_list_add_media(ml, node);
    queue_expected_item(&check, node);
    // Add items to that node
    libvlc_media_list_t *subitems = libvlc_media_subitems(node);
    queue_expected_item(&check, media_list_add_file_path(vlc, subitems, file));
    queue_expected_item(&check, media_list_add_file_path(vlc, subitems, file));
    queue_expected_item(&check, media_list_add_file_path(vlc, subitems, file));
    libvlc_media_list_release(subitems);
    libvlc_media_list_player_set_media_list (mlp, ml);
    libvlc_event_manager_t * em = libvlc_media_list_player_event_manager(mlp);
    int val = libvlc_event_attach(em, libvlc_MediaListPlayerNextItemSet,
                                  check_items_order_callback, &check);
    assert(val == 0);
    libvlc_media_list_player_play(mlp);
    // Wait until all item are read
    wait_queued_items(&check);
    stop_and_wait (mlp);
    libvlc_media_list_player_release (mlp);
    libvlc_release (vlc);
}
