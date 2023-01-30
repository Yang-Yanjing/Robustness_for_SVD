 *****************************************************************************/
void Close( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    vlc_join( p_sys->thread, NULL );
    free( p_sys );
}
