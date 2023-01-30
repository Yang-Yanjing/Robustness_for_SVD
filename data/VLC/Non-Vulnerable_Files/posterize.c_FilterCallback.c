}
static int FilterCallback ( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    (void)p_this; (void)oldval;
    filter_sys_t *p_sys = p_data;
    if( !strcmp( psz_var, CFG_PREFIX "level" ) )
        atomic_store( &p_sys->i_level, newval.i_int );
    return VLC_SUCCESS;
}
