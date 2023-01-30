}
static void PlayPause(intf_thread_t *intf)
{
    input_thread_t *p_input = intf->p_sys->p_input;
    if (p_input) {
        int64_t state = var_GetInteger( p_input, "state" );
        state = (state != PLAYING_S) ? PLAYING_S : PAUSE_S;
        var_SetInteger( p_input, "state", state );
    } else
        playlist_Play(pl_Get(intf));
}
