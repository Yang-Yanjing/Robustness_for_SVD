 *****************************************************************************/
static void Deactivate( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    vlc_cancel( p_sys->thread );
    var_DelCallback( p_sys->p_playlist, "volume", VolumeChanged, p_intf );
    vlc_join( p_sys->thread, NULL );
    if( p_sys->p_input != NULL )
    {
        var_DelCallback( p_sys->p_input, "intf-event", InputEvent, p_intf );
        vlc_object_release( p_sys->p_input );
    }
    net_ListenClose( p_sys->pi_socket_listen );
    if( p_sys->i_socket != -1 )
        net_Close( p_sys->i_socket );
    if( p_sys->psz_unix_path != NULL )
    {
#if defined(AF_LOCAL) && !defined(_WIN32)
        unlink( p_sys->psz_unix_path );
#endif
        free( p_sys->psz_unix_path );
    }
    vlc_mutex_destroy( &p_sys->status_lock );
    free( p_sys );
}
