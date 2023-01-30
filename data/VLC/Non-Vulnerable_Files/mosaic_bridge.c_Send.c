}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    picture_t *p_pic;
    if ( (sout_stream_sys_t *)id != p_sys )
    {
        block_ChainRelease( p_buffer );
        return VLC_SUCCESS;
    }
    while ( (p_pic = p_sys->p_decoder->pf_decode_video( p_sys->p_decoder,
                                                        &p_buffer )) )
    {
        picture_t *p_new_pic;
        if( p_sys->i_height || p_sys->i_width )
        {
            video_format_t fmt_out, fmt_in;
            memset( &fmt_in, 0, sizeof(video_format_t) );
            memset( &fmt_out, 0, sizeof(video_format_t) );
            fmt_in = p_sys->p_decoder->fmt_out.video;
            if( p_sys->i_chroma )
                fmt_out.i_chroma = p_sys->i_chroma;
            else
                fmt_out.i_chroma = VLC_CODEC_I420;
            const unsigned i_fmt_in_aspect =
                (int64_t)VOUT_ASPECT_FACTOR *
                fmt_in.i_sar_num * fmt_in.i_width /
                (fmt_in.i_sar_den * fmt_in.i_height);
            if ( !p_sys->i_height )
            {
                fmt_out.i_width = p_sys->i_width;
                fmt_out.i_height = (p_sys->i_width * VOUT_ASPECT_FACTOR
                    * p_sys->i_sar_num / p_sys->i_sar_den / i_fmt_in_aspect)
                      & ~0x1;
            }
            else if ( !p_sys->i_width )
            {
                fmt_out.i_height = p_sys->i_height;
                fmt_out.i_width = (p_sys->i_height * i_fmt_in_aspect
                    * p_sys->i_sar_den / p_sys->i_sar_num / VOUT_ASPECT_FACTOR)
                      & ~0x1;
            }
            else
            {
                fmt_out.i_width = p_sys->i_width;
                fmt_out.i_height = p_sys->i_height;
            }
            fmt_out.i_visible_width = fmt_out.i_width;
            fmt_out.i_visible_height = fmt_out.i_height;
            p_new_pic = image_Convert( p_sys->p_image,
                                       p_pic, &fmt_in, &fmt_out );
            if( p_new_pic == NULL )
            {
                msg_Err( p_stream, "image conversion failed" );
                picture_Release( p_pic );
                continue;
            }
        }
        else
        {
            /* TODO: chroma conversion if needed */
            p_new_pic = picture_New( p_pic->format.i_chroma,
                                     p_pic->format.i_width, p_pic->format.i_height,
                                     p_sys->p_decoder->fmt_out.video.i_sar_num,
                                     p_sys->p_decoder->fmt_out.video.i_sar_den );
            if( !p_new_pic )
            {
                picture_Release( p_pic );
                msg_Err( p_stream, "image allocation failed" );
                continue;
            }
            picture_Copy( p_new_pic, p_pic );
        }
        picture_Release( p_pic );
        if( p_sys->p_vf2 )
            p_new_pic = filter_chain_VideoFilter( p_sys->p_vf2, p_new_pic );
        PushPicture( p_stream, p_new_pic );
    }
    return VLC_SUCCESS;
}
