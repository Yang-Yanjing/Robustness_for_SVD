}
static int EraseCallback( vlc_object_t *p_this, char const *psz_var,
                          vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    if( !strcmp( psz_var, CFG_PREFIX "x" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        p_sys->i_x = newval.i_int;
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( !strcmp( psz_var, CFG_PREFIX "y" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        p_sys->i_y = newval.i_int;
        vlc_mutex_unlock( &p_sys->lock );
    }
    else if( !strcmp( psz_var, CFG_PREFIX "mask" ) )
    {
        vlc_mutex_lock( &p_sys->lock );
        LoadMask( (filter_t*)p_this, newval.psz_string );
        vlc_mutex_unlock( &p_sys->lock );
    }
    else
    {
        msg_Warn( p_this, "Unknown callback command." );
    }
    return VLC_SUCCESS;
}
