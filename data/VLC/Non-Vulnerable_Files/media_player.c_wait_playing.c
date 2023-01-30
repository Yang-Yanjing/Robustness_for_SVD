#include "test.h"
static void wait_playing(libvlc_media_player_t *mp)
{
    libvlc_state_t state;
    do {
        state = libvlc_media_player_get_state (mp);
    } while(state != libvlc_Playing &&
            state != libvlc_Error &&
            state != libvlc_Ended );
    state = libvlc_media_player_get_state (mp);
    assert(state == libvlc_Playing || state == libvlc_Ended);
}
