 *****************************************************************************/
static int LogoCallback( vlc_object_t *p_this, char const *psz_var,
                         vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval);
    filter_sys_t *p_sys = (filter_sys_t *)p_data;
    logo_list_t *p_list = &p_sys->list;
    vlc_mutex_lock( &p_sys->lock );
    if( !strcmp( psz_var, "logo-file" ) )
    {
        LogoListUnload( p_list );
        LogoListLoad( p_this, p_list, newval.psz_string );
    }
    else if ( !strcmp( psz_var, "logo-x" ) )
    {
        p_sys->i_pos_x = newval.i_int;
    }
    else if ( !strcmp( psz_var, "logo-y" ) )
    {
        p_sys->i_pos_y = newval.i_int;
    }
    else if ( !strcmp( psz_var, "logo-position" ) )
    {
        p_sys->i_pos = newval.i_int;
    }
    else if ( !strcmp( psz_var, "logo-opacity" ) )
    {
        p_list->i_alpha = VLC_CLIP( newval.i_int, 0, 255 );
    }
    else if ( !strcmp( psz_var, "logo-repeat" ) )
    {
        p_list->i_repeat = newval.i_int;
    }
    p_sys->b_spu_update = true;
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
