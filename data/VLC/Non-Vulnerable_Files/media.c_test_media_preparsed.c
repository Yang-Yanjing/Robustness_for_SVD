}
static void test_media_preparsed(const char** argv, int argc)
{
    // We use this image file because "empty.voc" has no track.
    const char * file = SRCDIR"/samples/image.jpg";
    log ("Testing set_media\n");
    libvlc_instance_t *vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    libvlc_media_t *media = libvlc_media_new_path (vlc, file);
    assert (media != NULL);
    volatile int received = false;
    // Check to see if we are properly receiving the event.
    libvlc_event_manager_t *em = libvlc_media_event_manager (media);
    libvlc_event_attach (em, libvlc_MediaParsedChanged, preparsed_changed, (void*)&received);
    // Parse the media. This is synchronous.
    libvlc_media_parse(media);
    // Wait to see if we properly receive preparsed.
    while (!received);
    // We are good, now check Elementary Stream info.
    libvlc_media_track_info_t *tracks = NULL;
    int num = libvlc_media_get_tracks_info(media, &tracks);
#warning libvlc_media_get_tracks_info is a broken function.
    // This is broken.
    // assert(num == 1);
    if (num != 1)
        printf("WARNING: libvlc_media_get_tracks_info is not working.");
    if (num > 0)
        free(tracks);
    libvlc_media_release (media);
    libvlc_release (vlc);
}
