/* updates handlers */
static void desktopResizeHandler( rdpContext *p_context )
{
    vlcrdp_context_t * p_vlccontext = (vlcrdp_context_t *) p_context;
    demux_sys_t *p_sys = p_vlccontext->p_demux->p_sys;
    rdpGdi *p_gdi = p_context->gdi;
    if ( p_sys->es )
    {
        es_out_Del( p_vlccontext->p_demux->out, p_sys->es );
        p_sys->es = NULL;
    }
    /* Now init and fill es format */
    vlc_fourcc_t i_chroma;
    switch( p_gdi->bytesPerPixel )
    {
        default:
        case 16:
            i_chroma = VLC_CODEC_RGB16;
            break;
        case 24:
            i_chroma = VLC_CODEC_RGB24;
            break;
        case 32:
            i_chroma = VLC_CODEC_RGB32;
            break;
    }
    es_format_t fmt;
    es_format_Init( &fmt, VIDEO_ES, i_chroma );
    fmt.video.i_chroma = i_chroma;
    fmt.video.i_visible_width =
    fmt.video.i_width = p_gdi->width;
    fmt.video.i_visible_height =
    fmt.video.i_height = p_gdi->height;
    fmt.video.i_frame_rate_base = 1000;
    fmt.video.i_frame_rate = 1000 * p_sys->f_fps;
    p_sys->i_framebuffersize = p_gdi->width * p_gdi->height * p_gdi->bytesPerPixel;
    if ( p_sys->p_block )
        p_sys->p_block = block_Realloc( p_sys->p_block, 0, p_sys->i_framebuffersize );
    else
        p_sys->p_block = block_Alloc( p_sys->i_framebuffersize );
    p_sys->es = es_out_Add( p_vlccontext->p_demux->out, &fmt );
}
