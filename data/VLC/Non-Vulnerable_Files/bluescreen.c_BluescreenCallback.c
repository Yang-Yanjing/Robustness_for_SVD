*****************************************************************************/
static int BluescreenCallback( vlc_object_t *p_this, char const *psz_var,
                               vlc_value_t oldval, vlc_value_t newval,
                               void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *) p_data;
    vlc_mutex_lock( &p_sys->lock );
#define VAR_IS( a ) !strcmp( psz_var, CFG_PREFIX a )
    if( VAR_IS( "u" ) )
        p_sys->i_u = VLC_CLIP( newval.i_int, 0, 255 );
    else if( VAR_IS( "v" ) )
        p_sys->i_v = VLC_CLIP( newval.i_int, 0, 255 );
    else if( VAR_IS( "ut" ) )
        p_sys->i_ut = VLC_CLIP( newval.i_int, 0, 255 );
    else if( VAR_IS( "vt" ) )
        p_sys->i_vt = VLC_CLIP( newval.i_int, 0, 255 );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
