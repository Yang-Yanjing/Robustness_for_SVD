static double find_peak_magnitude(AVFrame *frame, int channel)
{
    double max = DBL_EPSILON;
    int c, i;
    if (channel == -1) {
        for (c = 0; c < av_frame_get_channels(frame); c++) {
            double *data_ptr = (double *)frame->extended_data[c];
            for (i = 0; i < frame->nb_samples; i++)
                max = FFMAX(max, fabs(data_ptr[i]));
        }
    } else {
        double *data_ptr = (double *)frame->extended_data[channel];
        for (i = 0; i < frame->nb_samples; i++)
            max = FFMAX(max, fabs(data_ptr[i]));
    }
    return max;
}
