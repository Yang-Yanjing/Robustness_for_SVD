}
static block_t *Encode(encoder_t *enc, block_t *buf)
{
    encoder_sys_t *sys = enc->p_sys;
    if (!buf)
        return NULL;
    mtime_t i_pts = buf->i_pts -
                (mtime_t) CLOCK_FREQ * (mtime_t) sys->i_samples_delay /
                (mtime_t) enc->fmt_in.audio.i_rate;
    sys->i_samples_delay += buf->i_nb_samples;
    block_t *result = NULL;
    unsigned src_start = 0;
    unsigned padding_start = 0;
    /* The maximum Opus frame size is 1275 bytes + TOC sequence length. */
    const unsigned OPUS_MAX_ENCODED_BYTES = ((1275 + 3) * sys->nb_streams) - 2;
    while (sys->i_nb_samples + buf->i_nb_samples >= OPUS_FRAME_SIZE)
    {
        block_t *out_block = block_Alloc(OPUS_MAX_ENCODED_BYTES);
        /* add padding to beginning */
        if (sys->padding)
        {
            const size_t leftover_space = OPUS_FRAME_SIZE - sys->i_nb_samples;
            padding_start = fill_buffer(enc, padding_start, sys->padding,
                    __MIN(sys->padding->i_nb_samples, leftover_space));
            if (sys->padding->i_nb_samples <= 0)
            {
                block_Release(sys->padding);
                sys->padding = NULL;
            }
        }
        /* padding may have been freed either before or inside previous
         * if-statement */
        if (!sys->padding)
        {
            const size_t leftover_space = OPUS_FRAME_SIZE - sys->i_nb_samples;
            src_start = fill_buffer(enc, src_start, buf,
                    __MIN(buf->i_nb_samples, leftover_space));
        }
        opus_int32 bytes_encoded = opus_multistream_encode_float(sys->enc, sys->buffer,
                OPUS_FRAME_SIZE, out_block->p_buffer, out_block->i_buffer);
        if (bytes_encoded < 0)
        {
            block_Release(out_block);
        }
        else
        {
            out_block->i_length = (mtime_t) CLOCK_FREQ *
                (mtime_t) OPUS_FRAME_SIZE / (mtime_t) enc->fmt_in.audio.i_rate;
            out_block->i_dts = out_block->i_pts = i_pts;
            sys->i_samples_delay -= OPUS_FRAME_SIZE;
            i_pts += out_block->i_length;
            sys->i_nb_samples = 0;
            out_block->i_buffer = bytes_encoded;
            block_ChainAppend(&result, out_block);
        }
    }
    /* put leftover samples at beginning of buffer */
    if (buf->i_nb_samples > 0)
        fill_buffer(enc, src_start, buf, buf->i_nb_samples);
    return result;
}
