static void amplify_frame(DynamicAudioNormalizerContext *s, AVFrame *frame)
{
    int c, i;
    for (c = 0; c < s->channels; c++) {
        double *dst_ptr = (double *)frame->extended_data[c];
        double current_amplification_factor;
        cqueue_dequeue(s->gain_history_smoothed[c], &current_amplification_factor);
        for (i = 0; i < frame->nb_samples; i++) {
            const double amplification_factor = fade(s->prev_amplification_factor[c],
                                                     current_amplification_factor, i,
                                                     s->fade_factors);
            dst_ptr[i] *= amplification_factor;
            if (fabs(dst_ptr[i]) > s->peak_value)
                dst_ptr[i] = copysign(s->peak_value, dst_ptr[i]);
        }
        s->prev_amplification_factor[c] = current_amplification_factor;
    }
}
