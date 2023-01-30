 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys;
    p_sys = malloc( sizeof( intf_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_intf->p_sys = p_sys;
    p_sys->p_last_vout = NULL;
    p_sys->subtitle_delaybookmarks.i_time_audio = 0;
    p_sys->subtitle_delaybookmarks.i_time_subtitle = 0;
    var_AddCallback( p_intf->p_libvlc, "key-action", ActionEvent, p_intf );
    return VLC_SUCCESS;
}
