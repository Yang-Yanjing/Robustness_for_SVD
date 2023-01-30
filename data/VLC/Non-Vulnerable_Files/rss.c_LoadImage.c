 ***************************************************************************/
static picture_t *LoadImage( filter_t *p_filter, const char *psz_url )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    video_format_t fmt_in;
    video_format_t fmt_out;
    picture_t *p_orig;
    picture_t *p_pic = NULL;
    image_handler_t *p_handler = image_HandlerCreate( p_filter );
    memset( &fmt_in, 0, sizeof(video_format_t) );
    memset( &fmt_out, 0, sizeof(video_format_t) );
    fmt_out.i_chroma = VLC_CODEC_YUVA;
    p_orig = image_ReadUrl( p_handler, psz_url, &fmt_in, &fmt_out );
    if( !p_orig )
    {
        msg_Warn( p_filter, "Unable to read image %s", psz_url );
    }
    else if( p_sys->p_style->i_font_size > 0 )
    {
        fmt_in.i_chroma = VLC_CODEC_YUVA;
        fmt_in.i_height = p_orig->p[Y_PLANE].i_visible_lines;
        fmt_in.i_width = p_orig->p[Y_PLANE].i_visible_pitch;
        fmt_out.i_width = p_orig->p[Y_PLANE].i_visible_pitch
            *p_sys->p_style->i_font_size/p_orig->p[Y_PLANE].i_visible_lines;
        fmt_out.i_height = p_sys->p_style->i_font_size;
        p_pic = image_Convert( p_handler, p_orig, &fmt_in, &fmt_out );
        picture_Release( p_orig );
        if( !p_pic )
        {
            msg_Warn( p_filter, "Error while converting %s", psz_url );
        }
    }
    else
    {
        p_pic = p_orig;
    }
    image_HandlerDelete( p_handler );
    return p_pic;
}
