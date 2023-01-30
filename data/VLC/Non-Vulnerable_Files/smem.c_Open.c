 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    char* psz_tmp;
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    p_sys = calloc( 1, sizeof( sout_stream_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_stream->p_sys = p_sys;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                       p_stream->p_cfg );
    p_sys->time_sync = var_GetBool( p_stream, SOUT_CFG_PREFIX "time-sync" );
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_VIDEO "prerender-callback" );
    p_sys->pf_video_prerender_callback = (void (*) (void *, uint8_t**, size_t))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_AUDIO "prerender-callback" );
    p_sys->pf_audio_prerender_callback = (void (*) (void* , uint8_t**, size_t))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_VIDEO "postrender-callback" );
    p_sys->pf_video_postrender_callback = (void (*) (void*, uint8_t*, int, int, int, size_t, mtime_t))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );
    psz_tmp = var_GetString( p_stream, SOUT_PREFIX_AUDIO "postrender-callback" );
    p_sys->pf_audio_postrender_callback = (void (*) (void*, uint8_t*, unsigned int, unsigned int, unsigned int, unsigned int, size_t, mtime_t))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );
    /* Setting stream out module callbacks */
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->pace_nocontrol = p_sys->time_sync;
    return VLC_SUCCESS;
}
