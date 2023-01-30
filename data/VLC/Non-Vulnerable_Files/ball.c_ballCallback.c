 *****************************************************************************/
static int ballCallback( vlc_object_t *p_this, char const *psz_var,
                             vlc_value_t oldval, vlc_value_t newval,
                             void *p_data )
{
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    msg_Err( p_this, "Test" );
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, FILTER_PREFIX "color" ) )
    {
        p_sys->ballColor = getBallColor( p_this, newval.psz_string );
    }
    else if( !strcmp( psz_var, FILTER_PREFIX "size" ) )
    {
        p_sys->i_ballSize = newval.i_int;
    }
    else if( !strcmp( psz_var, FILTER_PREFIX "speed" ) )
    {
        p_sys->i_ballSpeed = newval.i_int;
    }
    else if( !strcmp( psz_var, FILTER_PREFIX "edge-visible" ) )
    {
        p_sys->b_edgeVisible = newval.b_bool;
    }
    else if( !strcmp( psz_var, FILTER_PREFIX "gradient-threshold" ) )
    {
        p_sys->i_gradThresh = newval.i_int;
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
