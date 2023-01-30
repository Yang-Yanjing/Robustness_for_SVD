 *****************************************************************************/
static void ProcessHeader(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    bs_t bs;
    int i_extra = p_dec->fmt_in.i_extra;
    char *p_extra = p_dec->fmt_in.p_extra;
    if (i_extra > 8 && !memcmp(p_extra, "fLaC", 4)) {
        i_extra -= 8;
        p_extra += 8;
    }
    if (p_dec->fmt_in.i_extra < 14)
        return;
    bs_init(&bs, p_extra, i_extra);
    p_sys->stream_info.min_blocksize = bs_read(&bs, 16);
    p_sys->stream_info.min_blocksize =
            __MIN( __MAX( p_sys->stream_info.min_blocksize, 16 ), 65535 );
    p_sys->stream_info.max_blocksize = bs_read(&bs, 16);
    p_sys->stream_info.max_blocksize =
                __MAX( __MIN( p_sys->stream_info.max_blocksize, 65535 ), 16 );
    p_sys->stream_info.min_framesize = bs_read(&bs, 24);
    p_sys->stream_info.min_framesize =
            __MAX( p_sys->stream_info.min_framesize, MIN_FLAC_FRAME_SIZE );
    p_sys->stream_info.max_framesize = bs_read(&bs, 24);
    p_sys->stream_info.sample_rate = bs_read(&bs, 20);
    p_sys->stream_info.channels = bs_read(&bs, 3) + 1;
    p_sys->stream_info.bits_per_sample = bs_read(&bs, 5) + 1;
    p_sys->b_stream_info = true;
    p_dec->fmt_out.i_extra = i_extra;
    free(p_dec->fmt_out.p_extra);
    p_dec->fmt_out.p_extra = malloc(i_extra);
    if (p_dec->fmt_out.p_extra)
        memcpy(p_dec->fmt_out.p_extra, p_extra, i_extra);
    else
        p_dec->fmt_out.i_extra = 0;
}
