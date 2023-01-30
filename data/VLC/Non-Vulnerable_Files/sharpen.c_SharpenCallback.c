}
static int SharpenCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval); VLC_UNUSED(psz_var);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    vlc_mutex_lock( &p_sys->lock );
    init_precalc_table( p_sys,  VLC_CLIP( newval.f_float, 0., 2. ) );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
