}
static int FilterCallback ( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    VLC_UNUSED(psz_var); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    atomic_store( &p_sys->i_intensity, newval.i_int );
    return VLC_SUCCESS;
}
