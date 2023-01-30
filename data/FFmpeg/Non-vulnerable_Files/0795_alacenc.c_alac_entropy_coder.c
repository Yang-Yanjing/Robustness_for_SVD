static void alac_entropy_coder(AlacEncodeContext *s, int ch)
{
    unsigned int history = s->rc.initial_history;
    int sign_modifier = 0, i, k;
    int32_t *samples = s->predictor_buf[ch];
    for (i = 0; i < s->frame_size;) {
        int x;
        k = av_log2((history >> 9) + 3);
        x  = -2 * (*samples) -1;
        x ^= x >> 31;
        samples++;
        i++;
        encode_scalar(s, x - sign_modifier, k, s->write_sample_size);
        history += x * s->rc.history_mult -
                   ((history * s->rc.history_mult) >> 9);
        sign_modifier = 0;
        if (x > 0xFFFF)
            history = 0xFFFF;
        if (history < 128 && i < s->frame_size) {
            unsigned int block_size = 0;
            k = 7 - av_log2(history) + ((history + 16) >> 6);
            while (*samples == 0 && i < s->frame_size) {
                samples++;
                i++;
                block_size++;
            }
            encode_scalar(s, block_size, k, 16);
            sign_modifier = (block_size <= 0xFFFF);
            history = 0;
        }
    }
}
