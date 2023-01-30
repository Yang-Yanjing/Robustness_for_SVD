}
static int AdjustCallback( vlc_object_t *p_this, char const *psz_var,
                           vlc_value_t oldval, vlc_value_t newval,
                           void *p_data )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    vlc_mutex_lock( &p_sys->lock );
    if( !strncmp( psz_var, "overlay-input", 13 ) )
    {
        free( p_sys->psz_inputfile );
        p_sys->psz_inputfile = strdup( newval.psz_string );
    }
    else if( !strncmp( psz_var, "overlay-output", 14 ) )
    {
        free( p_sys->psz_outputfile );
        p_sys->psz_outputfile = strdup( newval.psz_string );
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_EGENERIC;
}
