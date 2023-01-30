static void draw_sample_point(uint8_t *buf, int height, int linesize,
                              int16_t sample, int16_t *prev_y, int intensity)
{
    const int h = height/2 - av_rescale(sample, height/2, INT16_MAX);
    if (h >= 0 && h < height)
        buf[h * linesize] += intensity;
}
