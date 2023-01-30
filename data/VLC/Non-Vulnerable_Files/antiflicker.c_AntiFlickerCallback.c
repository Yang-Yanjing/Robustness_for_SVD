 *****************************************************************************/
static int AntiFlickerCallback( vlc_object_t *p_this, char const *psz_var,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    if( !strcmp( psz_var, FILTER_PREFIX "window-size" ) )
        atomic_store( &p_sys->i_window_size, newval.i_int );
    else if( !strcmp( psz_var, FILTER_PREFIX "softening-size" ) )
        atomic_store( &p_sys->i_softening, newval.i_int );
    return VLC_SUCCESS;
}
