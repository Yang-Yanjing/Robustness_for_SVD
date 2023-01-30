}
static encoder_t *CreateEncoder( vlc_object_t *p_this, video_format_t *fmt_in,
                                 video_format_t *fmt_out )
{
    encoder_t *p_enc;
    p_enc = sout_EncoderCreate( p_this );
    if( p_enc == NULL )
        return NULL;
    p_enc->p_module = NULL;
    es_format_Init( &p_enc->fmt_in, VIDEO_ES, fmt_in->i_chroma );
    p_enc->fmt_in.video = *fmt_in;
    if( p_enc->fmt_in.video.i_visible_width == 0 ||
        p_enc->fmt_in.video.i_visible_height == 0 ||
        p_enc->fmt_out.video.i_visible_width == 0 ||
        p_enc->fmt_out.video.i_visible_height == 0 )
    {
        if( fmt_out->i_width > 0 && fmt_out->i_height > 0 )
        {
            p_enc->fmt_in.video.i_width = fmt_out->i_width;
            p_enc->fmt_in.video.i_height = fmt_out->i_height;
            if( fmt_out->i_visible_width > 0 &&
                fmt_out->i_visible_height > 0 )
            {
                p_enc->fmt_in.video.i_visible_width = fmt_out->i_visible_width;
                p_enc->fmt_in.video.i_visible_height = fmt_out->i_visible_height;
            }
            else
            {
                p_enc->fmt_in.video.i_visible_width = fmt_out->i_width;
                p_enc->fmt_in.video.i_visible_height = fmt_out->i_height;
            }
        }
    } else if( fmt_out->i_sar_num && fmt_out->i_sar_den &&
               fmt_out->i_sar_num * fmt_in->i_sar_den !=
               fmt_out->i_sar_den * fmt_in->i_sar_num )
    {
        p_enc->fmt_in.video.i_width =
            fmt_in->i_sar_num * (int64_t)fmt_out->i_sar_den * fmt_in->i_width /
            fmt_in->i_sar_den / fmt_out->i_sar_num;
        p_enc->fmt_in.video.i_visible_width =
            fmt_in->i_sar_num * (int64_t)fmt_out->i_sar_den *
            fmt_in->i_visible_width / fmt_in->i_sar_den / fmt_out->i_sar_num;
    }
    p_enc->fmt_in.video.i_frame_rate = 25;
    p_enc->fmt_in.video.i_frame_rate_base = 1;
    es_format_Init( &p_enc->fmt_out, VIDEO_ES, fmt_out->i_chroma );
    p_enc->fmt_out.video = *fmt_out;
    p_enc->fmt_out.video.i_width = p_enc->fmt_in.video.i_width;
    p_enc->fmt_out.video.i_height = p_enc->fmt_in.video.i_height;
    /* Find a suitable decoder module */
    p_enc->p_module = module_need( p_enc, "encoder", NULL, false );
    if( !p_enc->p_module )
    {
        msg_Err( p_enc, "no suitable encoder module for fourcc `%4.4s'.\n"
                 "VLC probably does not support this image format.",
                 (char*)&p_enc->fmt_out.i_codec );
        DeleteEncoder( p_enc );
        return NULL;
    }
    p_enc->fmt_in.video.i_chroma = p_enc->fmt_in.i_codec;
    return p_enc;
}
