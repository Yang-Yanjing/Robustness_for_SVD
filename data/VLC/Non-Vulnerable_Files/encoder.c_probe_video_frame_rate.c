 *****************************************************************************/
static void probe_video_frame_rate( encoder_t *p_enc, AVCodecContext *p_context, AVCodec *p_codec )
{
    /* if we don't have i_frame_rate_base, we are probing and just checking if we can find codec
     * so set fps to requested fps if asked by user or input fps is availabled */
    p_context->time_base.num = p_enc->fmt_in.video.i_frame_rate_base ? p_enc->fmt_in.video.i_frame_rate_base : 1;
    // MP4V doesn't like CLOCK_FREQ denominator in time_base, so use 1/25 as default for that
    p_context->time_base.den = p_enc->fmt_in.video.i_frame_rate_base ? p_enc->fmt_in.video.i_frame_rate :
                                  ( p_enc->fmt_out.i_codec == VLC_CODEC_MP4V ? 25 : CLOCK_FREQ );
    msg_Dbg( p_enc, "Time base for probing setted to %d/%d", p_context->time_base.num, p_context->time_base.den );
    if( p_codec->supported_framerates )
    {
        /* We are finding fps values so 1/time_base */
        AVRational target = {
            .num = p_context->time_base.den,
            .den = p_context->time_base.num
        };
        int idx = av_find_nearest_q_idx(target, p_codec->supported_framerates);
        p_context->time_base.num = p_codec->supported_framerates[idx].den ?
                                    p_codec->supported_framerates[idx].den : 1;
        p_context->time_base.den = p_codec->supported_framerates[idx].den ?
                                    p_codec->supported_framerates[idx].num : CLOCK_FREQ;
        /* If we have something reasonable on supported framerates, use that*/
        if( p_context->time_base.den && p_context->time_base.den < CLOCK_FREQ )
        {
            p_enc->fmt_out.video.i_frame_rate_base =
                p_enc->fmt_in.video.i_frame_rate_base =
                p_context->time_base.num;
            p_enc->fmt_out.video.i_frame_rate =
                p_enc->fmt_in.video.i_frame_rate =
                p_context->time_base.den;
        }
    }
    msg_Dbg( p_enc, "Time base set to %d/%d", p_context->time_base.num, p_context->time_base.den );
}
