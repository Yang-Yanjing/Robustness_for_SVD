 */
void libvlc_Quit( libvlc_int_t *p_libvlc )
{
    vlc_exit_t *exit = &libvlc_priv( p_libvlc )->exit;
    msg_Dbg( p_libvlc, "exiting" );
    vlc_mutex_lock( &exit->lock );
    if( exit->handler != NULL )
        exit->handler( exit->opaque );
    else
        msg_Dbg( p_libvlc, "no exit handler" );
    vlc_mutex_unlock( &exit->lock );
}
