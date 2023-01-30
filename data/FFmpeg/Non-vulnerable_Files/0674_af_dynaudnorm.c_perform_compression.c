static void perform_compression(DynamicAudioNormalizerContext *s, AVFrame *frame)
{
    int is_first_frame = cqueue_empty(s->gain_history_original[0]);
    int c, i;
    if (s->channels_coupled) {
        const double standard_deviation = compute_frame_std_dev(s, frame, -1);
        const double current_threshold  = FFMIN(1.0, s->compress_factor * standard_deviation);
        const double prev_value = is_first_frame ? current_threshold : s->compress_threshold[0];
        double prev_actual_thresh, curr_actual_thresh;
        s->compress_threshold[0] = is_first_frame ? current_threshold : update_value(current_threshold, s->compress_threshold[0], (1.0/3.0));
        prev_actual_thresh = setup_compress_thresh(prev_value);
        curr_actual_thresh = setup_compress_thresh(s->compress_threshold[0]);
        for (c = 0; c < s->channels; c++) {
            double *const dst_ptr = (double *)frame->extended_data[c];
            for (i = 0; i < frame->nb_samples; i++) {
                const double localThresh = fade(prev_actual_thresh, curr_actual_thresh, i, s->fade_factors);
                dst_ptr[i] = copysign(bound(localThresh, fabs(dst_ptr[i])), dst_ptr[i]);
            }
        }
    } else {
        for (c = 0; c < s->channels; c++) {
            const double standard_deviation = compute_frame_std_dev(s, frame, c);
            const double current_threshold  = setup_compress_thresh(FFMIN(1.0, s->compress_factor * standard_deviation));
            const double prev_value = is_first_frame ? current_threshold : s->compress_threshold[c];
            double prev_actual_thresh, curr_actual_thresh;
            double *dst_ptr;
            s->compress_threshold[c] = is_first_frame ? current_threshold : update_value(current_threshold, s->compress_threshold[c], 1.0/3.0);
            prev_actual_thresh = setup_compress_thresh(prev_value);
            curr_actual_thresh = setup_compress_thresh(s->compress_threshold[c]);
            dst_ptr = (double *)frame->extended_data[c];
            for (i = 0; i < frame->nb_samples; i++) {
                const double localThresh = fade(prev_actual_thresh, curr_actual_thresh, i, s->fade_factors);
                dst_ptr[i] = copysign(bound(localThresh, fabs(dst_ptr[i])), dst_ptr[i]);
            }
        }
    }
}
