}
int main (void)
{
    test_init();
    // There are 6 tests. And they take some times.
    alarm(6 * 5);
    test_media_list_player_pause_stop (test_defaults_args, test_defaults_nargs);
    test_media_list_player_play_item_at_index (test_defaults_args, test_defaults_nargs);
    test_media_list_player_previous (test_defaults_args, test_defaults_nargs);
    test_media_list_player_next (test_defaults_args, test_defaults_nargs);
    test_media_list_player_items_queue (test_defaults_args, test_defaults_nargs);
    test_media_list_player_playback_options (test_defaults_args, test_defaults_nargs);
    return 0;
}
