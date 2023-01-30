 *****************************************************************************/
void Close ( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    motion_destroy( p_sys->p_motion );
    free( p_sys );
}
