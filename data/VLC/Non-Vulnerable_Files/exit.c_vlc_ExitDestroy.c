}
void vlc_ExitDestroy( vlc_exit_t *exit )
{
    vlc_mutex_destroy( &exit->lock );
}
