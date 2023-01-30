}
static int FilterCallback ( vlc_object_t *p_this, char const *psz_var,
                         vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    filter_sys_t *p_sys = p_data;
    if( !strcmp( psz_var, CFG_PREFIX "color" ) )
        atomic_store( &p_sys->i_color, newval.i_int );
    else if( !strcmp( psz_var, CFG_PREFIX "similaritythres" ) )
        atomic_store( &p_sys->i_simthres, newval.i_int );
    else /* CFG_PREFIX "saturationthres" */
        atomic_store( &p_sys->i_satthres, newval.i_int );
    (void)p_this; (void)oldval;
    return VLC_SUCCESS;
}
