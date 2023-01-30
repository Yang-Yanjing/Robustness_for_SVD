}
static int Statistics ( vlc_object_t *p_this, char const *psz_cmd,
    vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(newval); VLC_UNUSED(p_data);
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    input_thread_t *p_input =
        playlist_CurrentInput( p_intf->p_sys->p_playlist );
    if( !p_input )
        return VLC_ENOOBJ;
    updateStatistics( p_intf, input_GetItem(p_input) );
    vlc_object_release( p_input );
    return VLC_SUCCESS;
}
