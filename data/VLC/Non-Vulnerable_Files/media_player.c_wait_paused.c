}
static void wait_paused(libvlc_media_player_t *mp)
{
    libvlc_state_t state;
    do {
        state = libvlc_media_player_get_state (mp);
    } while(state != libvlc_Paused &&
            state != libvlc_Ended );
    state = libvlc_media_player_get_state (mp);
    assert(state == libvlc_Paused || state == libvlc_Ended);
}
