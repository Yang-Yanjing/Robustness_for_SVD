 ****************************************************************************/
static block_t *EncodeVideo( encoder_t *p_enc, picture_t *p_pict )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    int i_plane;
    AVFrame *frame = NULL;
    if( likely(p_pict) ) {
        frame = p_sys->frame;
        avcodec_get_frame_defaults( frame );
        for( i_plane = 0; i_plane < p_pict->i_planes; i_plane++ )
        {
            p_sys->frame->data[i_plane] = p_pict->p[i_plane].p_pixels;
            p_sys->frame->linesize[i_plane] = p_pict->p[i_plane].i_pitch;
        }
        /* Let libavcodec select the frame type */
        frame->pict_type = 0;
        frame->repeat_pict = p_pict->i_nb_fields - 2;
        frame->interlaced_frame = !p_pict->b_progressive;
        frame->top_field_first = !!p_pict->b_top_field_first;
        /* Set the pts of the frame being encoded
         * avcodec likes pts to be in time_base units
         * frame number */
        if( likely( p_pict->date > VLC_TS_INVALID ) )
            frame->pts = p_pict->date * p_sys->p_context->time_base.den /
                          CLOCK_FREQ / p_sys->p_context->time_base.num;
        else
            frame->pts = AV_NOPTS_VALUE;
        if ( p_sys->b_hurry_up && frame->pts != AV_NOPTS_VALUE )
        {
            mtime_t current_date = mdate();
            if ( current_date + HURRY_UP_GUARD3 > frame->pts )
            {
                p_sys->p_context->mb_decision = FF_MB_DECISION_SIMPLE;
                p_sys->p_context->trellis = 0;
                msg_Dbg( p_enc, "hurry up mode 3" );
            }
            else
            {
                p_sys->p_context->mb_decision = p_sys->i_hq;
                if ( current_date + HURRY_UP_GUARD2 > frame->pts )
                {
                    p_sys->p_context->trellis = 0;
                    p_sys->p_context->noise_reduction = p_sys->i_noise_reduction
                        + (HURRY_UP_GUARD2 + current_date - frame->pts) / 500;
                    msg_Dbg( p_enc, "hurry up mode 2" );
                }
                else
                {
                    p_sys->p_context->trellis = p_sys->b_trellis;
                    p_sys->p_context->noise_reduction =
                       p_sys->i_noise_reduction;
                }
            }
            if ( current_date + HURRY_UP_GUARD1 > frame->pts )
            {
                frame->pict_type = AV_PICTURE_TYPE_P;
                /* msg_Dbg( p_enc, "hurry up mode 1 %lld", current_date + HURRY_UP_GUARD1 - frame.pts ); */
            }
        }
        if ( frame->pts != AV_NOPTS_VALUE && frame->pts != 0 )
        {
            if ( p_sys->i_last_pts == frame->pts )
            {
                msg_Warn( p_enc, "almost fed libavcodec with two frames with "
                          "the same PTS (%"PRId64 ")", frame->pts );
                return NULL;
            }
            else if ( p_sys->i_last_pts > frame->pts )
            {
                msg_Warn( p_enc, "almost fed libavcodec with a frame in the "
                         "past (current: %"PRId64 ", last: %"PRId64")",
                         frame->pts, p_sys->i_last_pts );
                return NULL;
            }
            else
                p_sys->i_last_pts = frame->pts;
        }
        frame->quality = p_sys->i_quality;
    }
#if (LIBAVCODEC_VERSION_MAJOR >= 54)
    AVPacket av_pkt;
    av_pkt.data = NULL;
    av_pkt.size = 0;
    int is_data;
    av_init_packet( &av_pkt );
    if( avcodec_encode_video2( p_sys->p_context, &av_pkt, frame, &is_data ) < 0
     || is_data == 0 )
    {
        return NULL;
    }
    block_t *p_block = vlc_av_packet_Wrap( &av_pkt,
            av_pkt.duration / p_sys->p_context->time_base.den, p_sys->p_context );
    if( unlikely(p_block == NULL) )
    {
        av_free_packet( &av_pkt );
        return NULL;
    }
#else
    /* Initialize the video output buffer the first time.
     * This is done here instead of OpenEncoder() because we need the actual
     * bits_per_pixel value, without having to assume anything.
     */
    const int bitsPerPixel = p_enc->fmt_out.video.i_bits_per_pixel ?
                         p_enc->fmt_out.video.i_bits_per_pixel :
                         p_sys->p_context->bits_per_coded_sample ?
                         p_sys->p_context->bits_per_coded_sample :
                         24;
    unsigned blocksize = __MAX( FF_MIN_BUFFER_SIZE, ( bitsPerPixel * p_sys->p_context->height * p_sys->p_context->width ) / 8 + 200 );
    if( p_enc->fmt_out.i_codec == VLC_CODEC_TIFF )
    {
        blocksize = 2 * blocksize +
            4 * p_sys->p_context->height; /* space for strip sizes */
    }
    block_t *p_block = block_Alloc( blocksize );
    if( unlikely(p_block == NULL) )
        return NULL;
    int i_out = avcodec_encode_video( p_sys->p_context, p_block->p_buffer,
                                      p_block->i_buffer, frame );
    if( i_out <= 0 )
    {
        block_Release( p_block );
        return NULL;
    }
    p_block->i_buffer = i_out;
    /* FIXME, 3-2 pulldown is not handled correctly */
    p_block->i_length = INT64_C(1000000) *
        p_enc->fmt_in.video.i_frame_rate_base /
            p_enc->fmt_in.video.i_frame_rate;
    if( !p_sys->p_context->max_b_frames || !p_sys->p_context->delay )
    {
        /* No delay -> output pts == input pts */
        if( p_pict )
            p_block->i_dts = p_pict->date;
        p_block->i_pts = p_block->i_dts;
    }
    else if( p_sys->p_context->coded_frame->pts != AV_NOPTS_VALUE &&
        p_sys->p_context->coded_frame->pts != 0 &&
        p_sys->i_buggy_pts_detect != p_sys->p_context->coded_frame->pts )
    {
        p_sys->i_buggy_pts_detect = p_sys->p_context->coded_frame->pts;
        p_block->i_pts = p_sys->p_context->coded_frame->pts;
        if( p_sys->p_context->coded_frame->pict_type != AV_PICTURE_TYPE_I &&
            p_sys->p_context->coded_frame->pict_type != AV_PICTURE_TYPE_P )
        {
            p_block->i_dts = p_block->i_pts;
        }
        else
        {
            if( p_sys->i_last_ref_pts )
            {
                p_block->i_dts = p_sys->i_last_ref_pts;
            }
            else
            {
                /* Let's put something sensible */
                p_block->i_dts = p_block->i_pts;
            }
            p_sys->i_last_ref_pts = p_block->i_pts;
        }
    }
    else if( p_pict )
    {
        /* Buggy libavcodec which doesn't update coded_frame->pts
         * correctly */
        p_block->i_dts = p_block->i_pts = p_pict->date;
    }
#endif
    switch ( p_sys->p_context->coded_frame->pict_type )
    {
    case AV_PICTURE_TYPE_I:
    case AV_PICTURE_TYPE_SI:
        p_block->i_flags |= BLOCK_FLAG_TYPE_I;
        break;
    case AV_PICTURE_TYPE_P:
    case AV_PICTURE_TYPE_SP:
        p_block->i_flags |= BLOCK_FLAG_TYPE_P;
        break;
    case AV_PICTURE_TYPE_B:
    case AV_PICTURE_TYPE_BI:
        p_block->i_flags |= BLOCK_FLAG_TYPE_B;
        break;
    default:
        p_block->i_flags |= BLOCK_FLAG_TYPE_PB;
    }
    return p_block;
}
