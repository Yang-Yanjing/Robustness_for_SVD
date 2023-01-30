};
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    if( p_filter->fmt_in.video.i_chroma != VLC_CODEC_YUVA )
    {
        msg_Err( p_filter,
                 "Unsupported input chroma \"%4.4s\". "
                 "Bluescreen can only use \"YUVA\".",
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
    int val;
    vlc_mutex_init( &p_sys->lock );
#define GET_VAR( name, min, max )                                           \
    val = var_CreateGetIntegerCommand( p_filter, CFG_PREFIX #name );        \
    p_sys->i_##name = VLC_CLIP( val, min, max );                                \
    var_AddCallback( p_filter, CFG_PREFIX #name, BluescreenCallback, p_sys );
    GET_VAR( u, 0x00, 0xff );
    GET_VAR( v, 0x00, 0xff );
    GET_VAR( ut, 0x00, 0xff );
    GET_VAR( vt, 0x00, 0xff );
    p_sys->p_at = NULL;
#undef GET_VAR
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
