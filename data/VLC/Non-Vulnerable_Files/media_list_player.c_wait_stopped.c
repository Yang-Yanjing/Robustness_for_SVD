}
static inline void wait_stopped(libvlc_media_list_player_t *mlp)
{
    while (libvlc_media_list_player_is_playing (mlp))
        sched_yield();
}
