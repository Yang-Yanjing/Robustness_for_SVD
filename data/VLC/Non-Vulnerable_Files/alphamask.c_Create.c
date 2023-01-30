};
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_string;
    if( p_filter->fmt_in.video.i_chroma != VLC_CODEC_YUVA )
    {
        msg_Err( p_filter,
                 "Unsupported input chroma \"%4.4s\". "
                 "Alphamask can only use \"YUVA\".",
                 (char*)&p_filter->fmt_in.video.i_chroma );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys = p_filter->p_sys;
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    psz_string =
        var_CreateGetStringCommand( p_filter, CFG_PREFIX "mask" );
    if( psz_string && *psz_string )
    {
        LoadMask( p_filter, psz_string );
        if( !p_sys->p_mask )
            msg_Err( p_filter, "Error while loading mask (%s).",
                     psz_string );
    }
    else
       p_sys->p_mask = NULL;
    free( psz_string );
    vlc_mutex_init( &p_sys->mask_lock );
    var_AddCallback( p_filter, CFG_PREFIX "mask", MaskCallback,
                     p_filter );
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
