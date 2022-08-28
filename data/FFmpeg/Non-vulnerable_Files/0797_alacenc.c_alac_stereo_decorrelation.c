static void alac_stereo_decorrelation(AlacEncodeContext *s)
{
    int32_t *left = s->sample_buf[0], *right = s->sample_buf[1];
    int i, mode, n = s->frame_size;
    int32_t tmp;
    mode = estimate_stereo_mode(left, right, n);
    switch (mode) {
    case ALAC_CHMODE_LEFT_RIGHT:
        s->interlacing_leftweight = 0;
        s->interlacing_shift      = 0;
        break;
    case ALAC_CHMODE_LEFT_SIDE:
        for (i = 0; i < n; i++)
            right[i] = left[i] - right[i];
        s->interlacing_leftweight = 1;
        s->interlacing_shift      = 0;
        break;
    case ALAC_CHMODE_RIGHT_SIDE:
        for (i = 0; i < n; i++) {
            tmp = right[i];
            right[i] = left[i] - right[i];
            left[i]  = tmp + (right[i] >> 31);
        }
        s->interlacing_leftweight = 1;
        s->interlacing_shift      = 31;
        break;
    default:
        for (i = 0; i < n; i++) {
            tmp = left[i];
            left[i]  = (tmp + right[i]) >> 1;
            right[i] =  tmp - right[i];
        }
        s->interlacing_leftweight = 1;
        s->interlacing_shift      = 1;
        break;
    }
}
