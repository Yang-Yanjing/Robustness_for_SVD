static int flush_buffer(DynamicAudioNormalizerContext *s, AVFilterLink *inlink,
                        AVFilterLink *outlink)
{
    AVFrame *out = ff_get_audio_buffer(outlink, s->frame_len);
    int c, i;
    if (!out)
        return AVERROR(ENOMEM);
    for (c = 0; c < s->channels; c++) {
        double *dst_ptr = (double *)out->extended_data[c];
        for (i = 0; i < out->nb_samples; i++) {
            dst_ptr[i] = s->alt_boundary_mode ? DBL_EPSILON : ((s->target_rms > DBL_EPSILON) ? FFMIN(s->peak_value, s->target_rms) : s->peak_value);
            if (s->dc_correction) {
                dst_ptr[i] *= ((i % 2) == 1) ? -1 : 1;
                dst_ptr[i] += s->dc_correction_value[c];
            }
        }
    }
    s->delay--;
    return filter_frame(inlink, out);
}
