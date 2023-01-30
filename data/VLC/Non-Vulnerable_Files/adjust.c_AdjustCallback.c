}
static int AdjustCallback( vlc_object_t *p_this, char const *psz_var,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, "contrast" ) )
        p_sys->f_contrast = newval.f_float;
    else if( !strcmp( psz_var, "brightness" ) )
        p_sys->f_brightness = newval.f_float;
    else if( !strcmp( psz_var, "hue" ) )
        p_sys->i_hue = newval.i_int;
    else if( !strcmp( psz_var, "saturation" ) )
        p_sys->f_saturation = newval.f_float;
    else if( !strcmp( psz_var, "gamma" ) )
        p_sys->f_gamma = newval.f_float;
    else if( !strcmp( psz_var, "brightness-threshold" ) )
        p_sys->b_brightness_threshold = newval.b_bool;
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
