static double compute_frame_std_dev(DynamicAudioNormalizerContext *s,
                                    AVFrame *frame, int channel)
{
    double variance = 0.0;
    int i, c;
    if (channel == -1) {
        for (c = 0; c < s->channels; c++) {
            const double *data_ptr = (double *)frame->extended_data[c];
            for (i = 0; i < frame->nb_samples; i++) {
                variance += pow2(data_ptr[i]);  
            }
        }
        variance /= (s->channels * frame->nb_samples) - 1;
    } else {
        const double *data_ptr = (double *)frame->extended_data[channel];
        for (i = 0; i < frame->nb_samples; i++) {
            variance += pow2(data_ptr[i]);      
        }
        variance /= frame->nb_samples - 1;
    }
    return FFMAX(sqrt(variance), DBL_EPSILON);
}
