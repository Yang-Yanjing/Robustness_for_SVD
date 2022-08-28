static double compute_frame_rms(AVFrame *frame, int channel)
{
    double rms_value = 0.0;
    int c, i;
    if (channel == -1) {
        for (c = 0; c < av_frame_get_channels(frame); c++) {
            const double *data_ptr = (double *)frame->extended_data[c];
            for (i = 0; i < frame->nb_samples; i++) {
                rms_value += pow2(data_ptr[i]);
            }
        }
        rms_value /= frame->nb_samples * av_frame_get_channels(frame);
    } else {
        const double *data_ptr = (double *)frame->extended_data[channel];
        for (i = 0; i < frame->nb_samples; i++) {
            rms_value += pow2(data_ptr[i]);
        }
        rms_value /= frame->nb_samples;
    }
    return FFMAX(sqrt(rms_value), DBL_EPSILON);
}
