 */
static int OpenCommon( vlc_object_t *p_this, bool b_sub )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_filename;
    /* */
    if( !b_sub && !es_format_IsSimilar( &p_filter->fmt_in, &p_filter->fmt_out ) )
    {
        msg_Err( p_filter, "Input and output format does not match" );
        return VLC_EGENERIC;
    }
    /* */
    p_filter->p_sys = p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* */
    p_sys->p_blend = NULL;
    if( !b_sub )
    {
        p_sys->p_blend = filter_NewBlend( VLC_OBJECT(p_filter),
                                          &p_filter->fmt_in.video );
        if( !p_sys->p_blend )
        {
            free( p_sys );
            return VLC_EGENERIC;
        }
    }
    /* */
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    /* */
    logo_list_t *p_list = &p_sys->list;
    psz_filename = var_CreateGetStringCommand( p_filter, "logo-file" );
    if( !psz_filename )
    {
        if( p_sys->p_blend )
            filter_DeleteBlend( p_sys->p_blend );
        free( p_sys );
        return VLC_ENOMEM;
    }
    if( *psz_filename == '\0' )
        msg_Warn( p_this, "no logo file specified" );
    p_list->i_alpha = var_CreateGetIntegerCommand( p_filter, "logo-opacity");
    p_list->i_alpha = VLC_CLIP( p_list->i_alpha, 0, 255 );
    p_list->i_delay = var_CreateGetIntegerCommand( p_filter, "logo-delay" );
    p_list->i_repeat = var_CreateGetIntegerCommand( p_filter, "logo-repeat" );
    p_sys->i_pos = var_CreateGetIntegerCommand( p_filter, "logo-position" );
    p_sys->i_pos_x = var_CreateGetIntegerCommand( p_filter, "logo-x" );
    p_sys->i_pos_y = var_CreateGetIntegerCommand( p_filter, "logo-y" );
    p_sys->b_absolute = (p_sys->i_pos < 0);
    /* Ignore aligment if a position is given for video filter */
    if( !b_sub && p_sys->i_pos_x >= 0 && p_sys->i_pos_y >= 0 )
        p_sys->i_pos = 0;
    vlc_mutex_init( &p_sys->lock );
    LogoListLoad( p_this, p_list, psz_filename );
    p_sys->b_spu_update = true;
    p_sys->b_mouse_grab = false;
    for( int i = 0; ppsz_filter_callbacks[i]; i++ )
        var_AddCallback( p_filter, ppsz_filter_callbacks[i],
                         LogoCallback, p_sys );
    /* Misc init */
    if( b_sub )
    {
        p_filter->pf_sub_source = FilterSub;
    }
    else
    {
        p_filter->pf_video_filter = FilterVideo;
        p_filter->pf_video_mouse = Mouse;
    }
    free( psz_filename );
    return VLC_SUCCESS;
}
