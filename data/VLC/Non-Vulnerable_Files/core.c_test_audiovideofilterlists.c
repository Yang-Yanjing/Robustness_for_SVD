}
static void test_audiovideofilterlists (const char ** argv, int argc)
{
    libvlc_instance_t *vlc;
    log ("Testing libvlc_(audio|video)_filter_list_get()\n");
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    test_moduledescriptionlist (libvlc_audio_filter_list_get (vlc));
    test_moduledescriptionlist (libvlc_video_filter_list_get (vlc));
    libvlc_release (vlc);
}
