 ****************************************************************************/
static block_t *EncodeAudio(encoder_t *p_enc, block_t *p_aout_buf)
{
    int16_t *p_buffer;
    int i_samples;
    mtime_t i_pts_out;
    encoder_sys_t *p_sys = p_enc->p_sys;
    if (likely(p_aout_buf)) {
        p_buffer = (int16_t *)p_aout_buf->p_buffer;
        i_samples = p_aout_buf->i_nb_samples;
        i_pts_out = p_aout_buf->i_pts - (mtime_t)((double)CLOCK_FREQ *
               (double)p_sys->i_encoderdelay /
               (double)p_enc->fmt_out.audio.i_rate);
        if (p_sys->i_pts_last == 0)
            p_sys->i_pts_last = i_pts_out - (mtime_t)((double)CLOCK_FREQ *
               (double)(p_sys->i_frame_size) /
               (double)p_enc->fmt_out.audio.i_rate);
    } else {
        i_samples = 0;
        i_pts_out = p_sys->i_pts_last;
    }
    int i_samples_left = i_samples;
    int i_loop_count = 0;
    block_t *p_chain = NULL;
    while (i_samples_left >= 0) {
        AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
        AACENC_InArgs in_args = { 0 };
        AACENC_OutArgs out_args = { 0 };
        int in_identifier = IN_AUDIO_DATA;
        int in_size, in_elem_size;
        int out_identifier = OUT_BITSTREAM_DATA;
        int out_size, out_elem_size;
        void *in_ptr, *out_ptr;
        if (unlikely(i_samples == 0)) {
            // this forces the encoder to purge whatever is left in the internal buffer
            in_args.numInSamples = -1;
        } else {
            in_ptr = p_buffer + (i_samples - i_samples_left)*p_enc->fmt_in.audio.i_channels;
            in_size = 2*p_enc->fmt_in.audio.i_channels*i_samples_left;
            in_elem_size = 2;
            in_args.numInSamples = p_enc->fmt_in.audio.i_channels*i_samples_left;
            in_buf.numBufs = 1;
            in_buf.bufs = &in_ptr;
            in_buf.bufferIdentifiers = &in_identifier;
            in_buf.bufSizes = &in_size;
            in_buf.bufElSizes = &in_elem_size;
        }
        block_t *p_block;
        p_block = block_Alloc(p_sys->i_maxoutputsize);
        p_block->i_buffer = p_sys->i_maxoutputsize;
        out_ptr = p_block->p_buffer;
        out_size = p_block->i_buffer;
        out_elem_size = 1;
        out_buf.numBufs = 1;
        out_buf.bufs = &out_ptr;
        out_buf.bufferIdentifiers = &out_identifier;
        out_buf.bufSizes = &out_size;
        out_buf.bufElSizes = &out_elem_size;
        AACENC_ERROR erraac;
        if ((erraac = aacEncEncode(p_sys->handle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
            if (erraac == AACENC_ENCODE_EOF) {
                msg_Info(p_enc, "Encoding final bytes (EOF)");
            } else {
                msg_Err(p_enc, "Encoding failed: %s", fdkaac_error(erraac));
                block_Release(p_block);
                break;
            }
        }
        if (out_args.numOutBytes > 0) {
            p_block->i_buffer = out_args.numOutBytes;
            if (unlikely(i_samples == 0)) {
                // I only have the numOutBytes so approximate based on compression factor
                double d_samples_forward = p_sys->d_compression_ratio*(double)out_args.numOutBytes;
                i_pts_out += (mtime_t)d_samples_forward;
                p_block->i_length = (mtime_t)d_samples_forward;
                // TODO: It would be more precise (a few microseconds) to use d_samples_forward =
                // (mtime_t)CLOCK_FREQ * (mtime_t)p_sys->i_frame_size/(mtime_t)p_enc->fmt_out.audio.i_rate
                // but I am not sure if the lib always outputs a full frame when
                // emptying the internal buffer in the EOF scenario
            } else {
                if (i_loop_count == 0) {
                    // There can be an implicit delay in the first loop cycle because leftover bytes
                    // in the library buffer from the prior block
                    double d_samples_delay = (double)p_sys->i_frame_size - (double)out_args.numInSamples /
                                             (double)p_enc->fmt_in.audio.i_channels;
                    i_pts_out -= (mtime_t)((double)CLOCK_FREQ * d_samples_delay /
                                           (double)p_enc->fmt_out.audio.i_rate);
                    p_block->i_length = (mtime_t)((double)CLOCK_FREQ * (double)p_sys->i_frame_size /
                        (double)p_enc->fmt_out.audio.i_rate);
                    p_block->i_nb_samples = d_samples_delay;
                    //p_block->i_length = i_pts_out - p_sys->i_pts_last;
                } else {
                    double d_samples_forward = (double)out_args.numInSamples/(double)p_enc->fmt_in.audio.i_channels;
                    double d_length = ((double)CLOCK_FREQ * d_samples_forward /
                                            (double)p_enc->fmt_out.audio.i_rate);
                    i_pts_out += (mtime_t) d_length;
                    p_block->i_length = (mtime_t) d_length;
                    p_block->i_nb_samples = d_samples_forward;
                }
            }
            p_block->i_dts = p_block->i_pts = i_pts_out;
            block_ChainAppend(&p_chain, p_block);
#if 0
            msg_Dbg(p_enc, "dts %"PRId64", length %"PRId64", " "pts_last "
                            "%"PRId64" numOutBytes = %i, numInSamples = %i, "
                            "i_samples %i, i_loop_count %i",
                              p_block->i_dts, p_block->i_length,
                              p_sys->i_pts_last, out_args.numOutBytes,
                              out_args.numInSamples, i_samples, i_loop_count);
#endif
            if (likely(i_samples > 0)) {
                p_sys->d_compression_ratio = (double)p_block->i_length / (double)out_args.numOutBytes;
                i_samples_left -= out_args.numInSamples/p_enc->fmt_in.audio.i_channels;
                p_sys->i_pts_last = i_pts_out;
            }
        } else {
            block_Release(p_block);
            //msg_Dbg(p_enc, "aac_encode_audio: not enough data yet");
            break;
        }
        if (unlikely(i_loop_count++ > 100)) {
            msg_Err(p_enc, "Loop count greater than 100!!!, something must be wrong with the encoder library");
            break;
        }
    }
    return p_chain;
}
