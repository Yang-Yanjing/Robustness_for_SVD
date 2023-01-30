 *****************************************************************************/
static void DestroyFilter( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    msg_Dbg( p_filter, "DestroyFilter called." );
    stop_osdvnc( p_filter );
    var_DelCallback( p_filter->p_libvlc, "key-pressed", KeyEvent, p_this );
    var_Destroy( p_this, RMTOSD_CFG "host" );
    var_Destroy( p_this, RMTOSD_CFG "port" );
    var_Destroy( p_this, RMTOSD_CFG "password" );
    var_Destroy( p_this, RMTOSD_CFG "update" );
    var_Destroy( p_this, RMTOSD_CFG "vnc-polling" );
    var_Destroy( p_this, RMTOSD_CFG "mouse-events" );
    var_Destroy( p_this, RMTOSD_CFG "key-events" );
    var_Destroy( p_this, RMTOSD_CFG "alpha" );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys->psz_host );
    free( p_sys->psz_passwd );
    free( p_sys );
}
