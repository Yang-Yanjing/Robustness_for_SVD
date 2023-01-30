 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    intf_sys_t *p_sys = malloc( sizeof( *p_sys ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    p_intf->p_sys = p_sys;
    if( vlc_clone( &p_sys->thread, Run, p_intf, VLC_THREAD_PRIORITY_LOW ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
