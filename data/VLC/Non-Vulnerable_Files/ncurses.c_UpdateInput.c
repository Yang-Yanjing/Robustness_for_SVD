}
static inline void UpdateInput(intf_sys_t *sys, playlist_t *p_playlist)
{
    if (!sys->p_input) {
        sys->p_input = playlist_CurrentInput(p_playlist);
    } else if (sys->p_input->b_dead) {
        vlc_object_release(sys->p_input);
        sys->p_input = NULL;
    }
}
