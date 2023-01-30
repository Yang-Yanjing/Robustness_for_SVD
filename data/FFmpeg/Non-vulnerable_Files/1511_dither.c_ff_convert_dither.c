int ff_convert_dither(DitherContext *c, AudioData *dst, AudioData *src)
{
    int ret;
    AudioData *flt_data;
    
    if (dst->sample_fmt == AV_SAMPLE_FMT_S16P)
        c->s16_data = dst;
    else {
        
        ret = ff_audio_data_realloc(c->s16_data, src->nb_samples);
        if (ret < 0)
            return ret;
    }
    if (src->sample_fmt != AV_SAMPLE_FMT_FLTP || c->apply_map) {
        
        ret = ff_audio_data_realloc(c->flt_data, src->nb_samples);
        if (ret < 0)
            return ret;
        flt_data = c->flt_data;
    }
    if (src->sample_fmt != AV_SAMPLE_FMT_FLTP) {
        
        ret = ff_audio_convert(c->ac_in, flt_data, src);
        if (ret < 0)
            return ret;
    } else if (c->apply_map) {
        ret = ff_audio_data_copy(flt_data, src, c->ch_map_info);
        if (ret < 0)
            return ret;
    } else {
        flt_data = src;
    }
    
    if (c->method != AV_RESAMPLE_DITHER_TRIANGULAR_NS) {
        int ptr_align     = FFMIN(flt_data->ptr_align,     c->s16_data->ptr_align);
        int samples_align = FFMIN(flt_data->samples_align, c->s16_data->samples_align);
        int aligned_len   = FFALIGN(src->nb_samples, c->ddsp.samples_align);
        if (!(ptr_align % c->ddsp.ptr_align) && samples_align >= aligned_len) {
            c->quantize      = c->ddsp.quantize;
            c->samples_align = c->ddsp.samples_align;
        } else {
            c->quantize      = quantize_c;
            c->samples_align = 1;
        }
    }
    ret = convert_samples(c, (int16_t **)c->s16_data->data,
                          (float * const *)flt_data->data, src->channels,
                          src->nb_samples);
    if (ret < 0)
        return ret;
    c->s16_data->nb_samples = src->nb_samples;
    
    if (dst->sample_fmt == AV_SAMPLE_FMT_S16) {
        ret = ff_audio_convert(c->ac_out, dst, c->s16_data);
        if (ret < 0)
            return ret;
    } else
        c->s16_data = NULL;
    return 0;
}
