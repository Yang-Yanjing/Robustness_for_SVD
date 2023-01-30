#ifdef SCREEN_MOUSE
void RenderCursor( demux_t *p_demux, int i_x, int i_y,
                   uint8_t *p_dst )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( !p_sys->dst.i_planes )
        picture_Setup( &p_sys->dst, &p_sys->fmt.video );
    if( !p_sys->p_blend )
    {
        p_sys->p_blend = vlc_object_create( p_demux, sizeof(filter_t) );
        if( p_sys->p_blend )
        {
            es_format_Init( &p_sys->p_blend->fmt_in, VIDEO_ES,
                            VLC_CODEC_RGBA );
            p_sys->p_blend->fmt_in.video = p_sys->p_mouse->format;
            p_sys->p_blend->fmt_out = p_sys->fmt;
            p_sys->p_blend->p_module =
                module_need( p_sys->p_blend, "video blending", NULL, false );
            if( !p_sys->p_blend->p_module )
            {
                msg_Err( p_demux, "Could not load video blending module" );
                vlc_object_release( p_sys->p_blend );
                p_sys->p_blend = NULL;
            }
        }
    }
    if( p_sys->p_blend )
    {
        p_sys->dst.p->p_pixels = p_dst;
        p_sys->p_blend->pf_video_blend( p_sys->p_blend,
                                        &p_sys->dst,
                                        p_sys->p_mouse,
#ifdef SCREEN_SUBSCREEN
                                        i_x-p_sys->i_left,
#else
                                        i_x,
#endif
#ifdef SCREEN_SUBSCREEN
                                        i_y-p_sys->i_top,
#else
                                        i_y,
#endif
                                        255 );
    }
    else
    {
        picture_Release( p_sys->p_mouse );
        p_sys->p_mouse = NULL;
    }
}
