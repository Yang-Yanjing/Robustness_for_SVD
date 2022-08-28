static void analyze_frame(DynamicAudioNormalizerContext *s, AVFrame *frame)
{
    if (s->dc_correction) {
        perform_dc_correction(s, frame);
    }
    if (s->compress_factor > DBL_EPSILON) {
        perform_compression(s, frame);
    }
    if (s->channels_coupled) {
        const double current_gain_factor = get_max_local_gain(s, frame, -1);
        int c;
        for (c = 0; c < s->channels; c++)
            update_gain_history(s, c, current_gain_factor);
    } else {
        int c;
        for (c = 0; c < s->channels; c++)
            update_gain_history(s, c, get_max_local_gain(s, frame, c));
    }
}
