};
static unsigned fill_buffer(encoder_t *enc, unsigned src_start, block_t *src,
                            unsigned samples)
{
    encoder_sys_t *p_sys = enc->p_sys;
    const unsigned channels = enc->fmt_out.audio.i_channels;
    const float *src_buf = ((const float *) src->p_buffer) + src_start;
    float *dest_buf = p_sys->buffer + (p_sys->i_nb_samples * channels);
    const unsigned len = samples * channels;
    memcpy(dest_buf, src_buf, len * sizeof(float));
    p_sys->i_nb_samples += samples;
    src_start += len;
    src->i_nb_samples -= samples;
    return src_start;
}
