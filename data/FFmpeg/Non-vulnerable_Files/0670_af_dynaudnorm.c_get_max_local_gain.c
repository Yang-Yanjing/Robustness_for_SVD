static double get_max_local_gain(DynamicAudioNormalizerContext *s, AVFrame *frame,
                                 int channel)
{
    const double maximum_gain = s->peak_value / find_peak_magnitude(frame, channel);
    const double rms_gain = s->target_rms > DBL_EPSILON ? (s->target_rms / compute_frame_rms(frame, channel)) : DBL_MAX;
    return bound(s->max_amplification, FFMIN(maximum_gain, rms_gain));
}
