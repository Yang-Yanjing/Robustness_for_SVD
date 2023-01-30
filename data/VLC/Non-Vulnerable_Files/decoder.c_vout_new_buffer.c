}
static picture_t *vout_new_buffer( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    if( p_owner->p_vout == NULL ||
        p_dec->fmt_out.video.i_width != p_owner->video.i_width ||
        p_dec->fmt_out.video.i_height != p_owner->video.i_height ||
        p_dec->fmt_out.video.i_visible_width != p_owner->video.i_visible_width ||
        p_dec->fmt_out.video.i_visible_height != p_owner->video.i_visible_height ||
        p_dec->fmt_out.video.i_x_offset != p_owner->video.i_x_offset  ||
        p_dec->fmt_out.video.i_y_offset != p_owner->video.i_y_offset  ||
        p_dec->fmt_out.i_codec != p_owner->video.i_chroma ||
        (int64_t)p_dec->fmt_out.video.i_sar_num * p_owner->video.i_sar_den !=
        (int64_t)p_dec->fmt_out.video.i_sar_den * p_owner->video.i_sar_num ||
        p_dec->fmt_out.video.orientation != p_owner->video.orientation )
    {
        vout_thread_t *p_vout;
        if( !p_dec->fmt_out.video.i_width ||
            !p_dec->fmt_out.video.i_height )
        {
            /* Can't create a new vout without display size */
            return NULL;
        }
        video_format_t fmt = p_dec->fmt_out.video;
        fmt.i_chroma = p_dec->fmt_out.i_codec;
        p_owner->video = fmt;
        if( vlc_fourcc_IsYUV( fmt.i_chroma ) )
        {
            const vlc_chroma_description_t *dsc = vlc_fourcc_GetChromaDescription( fmt.i_chroma );
            for( unsigned int i = 0; dsc && i < dsc->plane_count; i++ )
            {
                while( fmt.i_width % dsc->p[i].w.den )
                    fmt.i_width++;
                while( fmt.i_height % dsc->p[i].h.den )
                    fmt.i_height++;
            }
        }
        if( !fmt.i_visible_width || !fmt.i_visible_height )
        {
            if( p_dec->fmt_in.video.i_visible_width &&
                p_dec->fmt_in.video.i_visible_height )
            {
                fmt.i_visible_width  = p_dec->fmt_in.video.i_visible_width;
                fmt.i_visible_height = p_dec->fmt_in.video.i_visible_height;
                fmt.i_x_offset       = p_dec->fmt_in.video.i_x_offset;
                fmt.i_y_offset       = p_dec->fmt_in.video.i_y_offset;
            }
            else
            {
                fmt.i_visible_width  = fmt.i_width;
                fmt.i_visible_height = fmt.i_height;
                fmt.i_x_offset       = 0;
                fmt.i_y_offset       = 0;
            }
        }
        if( fmt.i_visible_height == 1088 &&
            var_CreateGetBool( p_dec, "hdtv-fix" ) )
        {
            fmt.i_visible_height = 1080;
            if( !(fmt.i_sar_num % 136))
            {
                fmt.i_sar_num *= 135;
                fmt.i_sar_den *= 136;
            }
            msg_Warn( p_dec, "Fixing broken HDTV stream (display_height=1088)");
        }
        if( !fmt.i_sar_num || !fmt.i_sar_den )
        {
            fmt.i_sar_num = 1;
            fmt.i_sar_den = 1;
        }
        vlc_ureduce( &fmt.i_sar_num, &fmt.i_sar_den,
                     fmt.i_sar_num, fmt.i_sar_den, 50000 );
        vlc_mutex_lock( &p_owner->lock );
        p_vout = p_owner->p_vout;
        p_owner->p_vout = NULL;
        vlc_mutex_unlock( &p_owner->lock );
        unsigned dpb_size;
        switch( p_dec->fmt_in.i_codec )
        {
        case VLC_CODEC_HEVC:
        case VLC_CODEC_H264:
        case VLC_CODEC_DIRAC: /* FIXME valid ? */
            dpb_size = 18;
            break;
        case VLC_CODEC_VP5:
        case VLC_CODEC_VP6:
        case VLC_CODEC_VP6F:
        case VLC_CODEC_VP8:
            dpb_size = 3;
            break;
        default:
            dpb_size = 2;
            break;
        }
        p_vout = input_resource_RequestVout( p_owner->p_resource,
                                             p_vout, &fmt,
                                             dpb_size +
                                             p_dec->i_extra_picture_buffers + 1,
                                             true );
        vlc_mutex_lock( &p_owner->lock );
        p_owner->p_vout = p_vout;
        DecoderUpdateFormatLocked( p_dec );
        vlc_mutex_unlock( &p_owner->lock );
        if( p_owner->p_input != NULL )
            input_SendEventVout( p_owner->p_input );
        if( p_vout == NULL )
        {
            msg_Err( p_dec, "failed to create video output" );
            p_dec->b_error = true;
            return NULL;
        }
    }
    /* Get a new picture
     */
    for( ;; )
    {
        if( DecoderIsExitRequested( p_dec ) || p_dec->b_error )
            return NULL;
        picture_t *p_picture = vout_GetPicture( p_owner->p_vout );
        if( p_picture )
            return p_picture;
        if( DecoderIsFlushing( p_dec ) )
            return NULL;
        /* */
        DecoderSignalWait( p_dec, true );
        /* Check the decoder doesn't leak pictures */
        vout_FixLeaks( p_owner->p_vout );
        /* FIXME add a vout_WaitPictureAvailable (timedwait) */
        msleep( VOUT_OUTMEM_SLEEP );
    }
}
