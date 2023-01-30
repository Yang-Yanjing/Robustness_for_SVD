}
static int ExtractCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, FILTER_PREFIX "component" ) )
    {
        p_sys->i_color = newval.i_int;
        /* Matrix won't be used for RED, GREEN or BLUE in planar formats */
        make_projection_matrix( (filter_t *)p_this, p_sys->i_color,
                                p_sys->projection_matrix );
    }
    else
    {
        msg_Warn( p_this, "Unknown callback command." );
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
