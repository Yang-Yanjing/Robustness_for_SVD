static void update_gain_history(DynamicAudioNormalizerContext *s, int channel,
                                double current_gain_factor)
{
    if (cqueue_empty(s->gain_history_original[channel]) ||
        cqueue_empty(s->gain_history_minimum[channel])) {
        const int pre_fill_size = s->filter_size / 2;
        s->prev_amplification_factor[channel] = s->alt_boundary_mode ? current_gain_factor : 1.0;
        while (cqueue_size(s->gain_history_original[channel]) < pre_fill_size) {
            cqueue_enqueue(s->gain_history_original[channel], s->alt_boundary_mode ? current_gain_factor : 1.0);
        }
        while (cqueue_size(s->gain_history_minimum[channel]) < pre_fill_size) {
            cqueue_enqueue(s->gain_history_minimum[channel], s->alt_boundary_mode ? current_gain_factor : 1.0);
        }
    }
    cqueue_enqueue(s->gain_history_original[channel], current_gain_factor);
    while (cqueue_size(s->gain_history_original[channel]) >= s->filter_size) {
        double minimum;
        av_assert0(cqueue_size(s->gain_history_original[channel]) == s->filter_size);
        minimum = minimum_filter(s->gain_history_original[channel]);
        cqueue_enqueue(s->gain_history_minimum[channel], minimum);
        cqueue_pop(s->gain_history_original[channel]);
    }
    while (cqueue_size(s->gain_history_minimum[channel]) >= s->filter_size) {
        double smoothed;
        av_assert0(cqueue_size(s->gain_history_minimum[channel]) == s->filter_size);
        smoothed = gaussian_filter(s, s->gain_history_minimum[channel]);
        cqueue_enqueue(s->gain_history_smoothed[channel], smoothed);
        cqueue_pop(s->gain_history_minimum[channel]);
    }
}
