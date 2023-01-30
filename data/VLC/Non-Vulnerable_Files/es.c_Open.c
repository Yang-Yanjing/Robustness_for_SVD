 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t       *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t   *p_sys;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options, p_stream->p_cfg );
    p_sys                   = malloc( sizeof( sout_stream_sys_t ) );
    p_sys->i_count          = 0;
    p_sys->i_count_audio    = 0;
    p_sys->i_count_video    = 0;
    p_sys->psz_access = var_GetString( p_stream, SOUT_CFG_PREFIX "access" );
    p_sys->psz_access_audio = var_GetString( p_stream, SOUT_CFG_PREFIX "access-audio" );
    p_sys->psz_access_video = var_GetString( p_stream, SOUT_CFG_PREFIX "access-video" );
    p_sys->psz_mux = var_GetString( p_stream, SOUT_CFG_PREFIX "mux" );
    p_sys->psz_mux_audio = var_GetString( p_stream, SOUT_CFG_PREFIX "mux-audio" );
    p_sys->psz_mux_video = var_GetString( p_stream, SOUT_CFG_PREFIX "mux-video" );
    p_sys->psz_dst       = var_GetString( p_stream, SOUT_CFG_PREFIX "dst" );
    p_sys->psz_dst_audio = var_GetString( p_stream, SOUT_CFG_PREFIX "dst-audio" );
    p_sys->psz_dst_video = var_GetString( p_stream, SOUT_CFG_PREFIX "dst-video" );
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys     = p_sys;
    return VLC_SUCCESS;
}
