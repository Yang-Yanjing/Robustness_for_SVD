 */
void libvlc_SetExitHandler( libvlc_int_t *p_libvlc, void (*handler) (void *),
                            void *opaque )
{
    vlc_exit_t *exit = &libvlc_priv( p_libvlc )->exit;
    vlc_mutex_lock( &exit->lock );
    exit->handler = handler;
    exit->opaque = opaque;
    vlc_mutex_unlock( &exit->lock );
}
