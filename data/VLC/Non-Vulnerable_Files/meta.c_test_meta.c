#include "test.h"
static void test_meta (const char ** argv, int argc)
{
    libvlc_instance_t *vlc;
    libvlc_media_t *media;
    char * artist;
    log ("Testing meta\n");
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    media = libvlc_media_new_path (vlc, "samples/meta.sample");
    assert( media );
    libvlc_media_parse (media);
    artist = libvlc_media_get_meta (media, libvlc_meta_Artist);
    const char *expected_artist = "mike";
    assert (artist);
    log ("+ got '%s' as Artist, expecting %s\n", artist, expected_artist);
    int string_compare = strcmp (artist, expected_artist);
    assert (!string_compare);
    free (artist);
    libvlc_media_release (media);
    libvlc_release (vlc);
}
