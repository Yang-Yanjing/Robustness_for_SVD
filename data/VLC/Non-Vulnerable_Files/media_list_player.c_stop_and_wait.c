}
static inline void stop_and_wait(libvlc_media_list_player_t *mlp)
{
    libvlc_media_list_player_stop (mlp);
    wait_stopped (mlp);
}
