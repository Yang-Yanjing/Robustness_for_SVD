#include <string.h>
static void test_core (const char ** argv, int argc)
{
    libvlc_instance_t *vlc;
    log ("Testing core\n");
    vlc = libvlc_new (argc, argv);
    assert (vlc != NULL);
    libvlc_retain (vlc);
    libvlc_release (vlc);
    libvlc_release (vlc);
}
