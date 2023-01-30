 *****************************************************************************/
static void Close ( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    /* Destroy the callbacks (the order matters!) */
    var_DelCallback( pl_Get(p_intf), "input-current", PlaylistEvent, p_intf );
    if( p_sys->p_input )
    {
        var_DelCallback( p_sys->p_input, "intf-event", InputEvent, p_intf );
        vlc_object_release( p_sys->p_input );
    }
    if( p_sys->p_vout )
    {
        var_DelCallback( p_sys->p_vout, "mouse-moved", MovedEvent, p_intf );
        var_DelCallback( p_sys->p_vout, "mouse-button-down",
                         ButtonEvent, p_intf );
        vlc_object_release( p_sys->p_vout );
    }
    /* Destroy structure */
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}
