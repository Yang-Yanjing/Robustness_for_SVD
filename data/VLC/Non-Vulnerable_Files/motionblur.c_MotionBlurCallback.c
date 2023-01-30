}
static int MotionBlurCallback( vlc_object_t *p_this, char const *psz_var,
                               vlc_value_t oldval, vlc_value_t newval,
                               void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(psz_var); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    atomic_store( &p_sys->i_factor, VLC_CLIP( newval.i_int, 1, 127 ) );
    return VLC_SUCCESS;
}
