static void draw_sample_p2p(uint8_t *buf, int height, int linesize,
                            int16_t sample, int16_t *prev_y, int intensity)
{
    int k;
    const int h = height/2 - av_rescale(sample, height/2, INT16_MAX);
    if (h >= 0 && h < height) {
        buf[h * linesize] += intensity;
        if (*prev_y && h != *prev_y) {
            int start = *prev_y;
            int end = av_clip(h, 0, height-1);
            if (start > end)
                FFSWAP(int16_t, start, end);
            for (k = start + 1; k < end; k++)
                buf[k * linesize] += intensity;
        }
    }
    *prev_y = h;
}
