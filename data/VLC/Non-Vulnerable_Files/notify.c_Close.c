 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    intf_thread_t   *p_intf = ( intf_thread_t* ) p_this;
    intf_sys_t      *p_sys  = p_intf->p_sys;
    var_DelCallback( pl_Get( p_this ), "activity", ItemChange, p_this );
    if( p_sys->notification )
    {
        GError *p_error = NULL;
        notify_notification_close( p_sys->notification, &p_error );
        g_object_unref( p_sys->notification );
    }
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
    notify_uninit();
}
