#include "../lib/libvlc_internal.h"
void vlc_ExitInit( vlc_exit_t *exit )
{
    vlc_mutex_init( &exit->lock );
    exit->handler = NULL;
    exit->opaque = NULL;
}
