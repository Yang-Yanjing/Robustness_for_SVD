int ff_audio_resample(ResampleContext *c, AudioData *dst, AudioData *src)
{
    int ch, in_samples, in_leftover, consumed = 0, out_samples = 0;
    int ret = AVERROR(EINVAL);
    int nearest_neighbour = (c->compensation_distance == 0 &&
                             c->filter_length == 1 &&
                             c->phase_shift == 0);
    in_samples  = src ? src->nb_samples : 0;
    in_leftover = c->buffer->nb_samples;
    
    if (src) {
        ret = ff_audio_data_combine(c->buffer, in_leftover, src, 0, in_samples);
        if (ret < 0)
            return ret;
    } else if (in_leftover <= c->final_padding_samples) {
        
        return 0;
    }
    if (!c->initial_padding_filled) {
        int bps = av_get_bytes_per_sample(c->avr->internal_sample_fmt);
        int i;
        if (src && c->buffer->nb_samples < 2 * c->padding_size)
            return 0;
        for (i = 0; i < c->padding_size; i++)
            for (ch = 0; ch < c->buffer->channels; ch++) {
                if (c->buffer->nb_samples > 2 * c->padding_size - i) {
                    memcpy(c->buffer->data[ch] + bps * i,
                           c->buffer->data[ch] + bps * (2 * c->padding_size - i), bps);
                } else {
                    memset(c->buffer->data[ch] + bps * i, 0, bps);
                }
            }
        c->initial_padding_filled = 1;
    }
    if (!src && !c->final_padding_filled) {
        int bps = av_get_bytes_per_sample(c->avr->internal_sample_fmt);
        int i;
        ret = ff_audio_data_realloc(c->buffer,
                                    FFMAX(in_samples, in_leftover) +
                                    c->padding_size);
        if (ret < 0) {
            av_log(c->avr, AV_LOG_ERROR, "Error reallocating resampling buffer\n");
            return AVERROR(ENOMEM);
        }
        for (i = 0; i < c->padding_size; i++)
            for (ch = 0; ch < c->buffer->channels; ch++) {
                if (in_leftover > i) {
                    memcpy(c->buffer->data[ch] + bps * (in_leftover + i),
                           c->buffer->data[ch] + bps * (in_leftover - i - 1),
                           bps);
                } else {
                    memset(c->buffer->data[ch] + bps * (in_leftover + i),
                           0, bps);
                }
            }
        c->buffer->nb_samples   += c->padding_size;
        c->final_padding_samples = c->padding_size;
        c->final_padding_filled  = 1;
    }
    
    
    if (!dst->read_only && dst->allow_realloc) {
        out_samples = resample(c, NULL, NULL, NULL, c->buffer->nb_samples,
                               INT_MAX, 0, nearest_neighbour);
        ret = ff_audio_data_realloc(dst, out_samples);
        if (ret < 0) {
            av_log(c->avr, AV_LOG_ERROR, "error reallocating output\n");
            return ret;
        }
    }
    
    for (ch = 0; ch < c->buffer->channels; ch++) {
        out_samples = resample(c, (void *)dst->data[ch],
                               (const void *)c->buffer->data[ch], &consumed,
                               c->buffer->nb_samples, dst->allocated_samples,
                               ch + 1 == c->buffer->channels, nearest_neighbour);
    }
    if (out_samples < 0) {
        av_log(c->avr, AV_LOG_ERROR, "error during resampling\n");
        return out_samples;
    }
    
    ff_audio_data_drain(c->buffer, consumed);
    c->initial_padding_samples = FFMAX(c->initial_padding_samples - consumed, 0);
    av_log(c->avr, AV_LOG_TRACE, "resampled %d in + %d leftover to %d out + %d leftover\n",
            in_samples, in_leftover, out_samples, c->buffer->nb_samples);
    dst->nb_samples = out_samples;
    return 0;
}
