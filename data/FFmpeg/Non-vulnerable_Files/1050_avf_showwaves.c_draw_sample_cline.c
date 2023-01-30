static void draw_sample_cline(uint8_t *buf, int height, int linesize,
                              int16_t sample, int16_t *prev_y, int intensity)
{
    int k;
    const int h     = av_rescale(abs(sample), height, INT16_MAX);
    const int start = (height - h) / 2;
    const int end   = start + h;
    for (k = start; k < end; k++)
        buf[k * linesize] += intensity;
}
