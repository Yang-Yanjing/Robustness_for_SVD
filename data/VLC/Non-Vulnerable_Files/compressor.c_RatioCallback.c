}
static int RatioCallback( vlc_object_t *p_this, char const *psz_cmd,
                          vlc_value_t oldval, vlc_value_t newval,
                          void * p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = p_data;
    vlc_mutex_lock( &p_sys->lock );
    p_sys->f_ratio = Clamp( newval.f_float, 1.0f, 20.0f );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}