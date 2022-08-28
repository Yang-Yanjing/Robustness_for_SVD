static av_cold int init_noise(NoiseContext *n, int comp)
{
    int8_t *noise = av_malloc(MAX_NOISE * sizeof(int8_t));
    FilterParams *fp = &n->param[comp];
    AVLFG *lfg = &n->param[comp].lfg;
    int strength = fp->strength;
    int flags = fp->flags;
    int i, j;
    if (!noise)
        return AVERROR(ENOMEM);
    av_lfg_init(&fp->lfg, fp->seed + comp*31415U);
    for (i = 0, j = 0; i < MAX_NOISE; i++, j++) {
        if (flags & NOISE_UNIFORM) {
            if (flags & NOISE_AVERAGED) {
                if (flags & NOISE_PATTERN) {
                    noise[i] = (RAND_N(strength) - strength / 2) / 6
                        + patt[j % 4] * strength * 0.25 / 3;
                } else {
                    noise[i] = (RAND_N(strength) - strength / 2) / 3;
                }
            } else {
                if (flags & NOISE_PATTERN) {
                    noise[i] = (RAND_N(strength) - strength / 2) / 2
                        + patt[j % 4] * strength * 0.25;
                } else {
                    noise[i] = RAND_N(strength) - strength / 2;
                }
            }
        } else {
            double x1, x2, w, y1;
            do {
                x1 = 2.0 * av_lfg_get(lfg) / (float)UINT_MAX - 1.0;
                x2 = 2.0 * av_lfg_get(lfg) / (float)UINT_MAX - 1.0;
                w = x1 * x1 + x2 * x2;
            } while (w >= 1.0);
            w   = sqrt((-2.0 * log(w)) / w);
            y1  = x1 * w;
            y1 *= strength / sqrt(3.0);
            if (flags & NOISE_PATTERN) {
                y1 /= 2;
                y1 += patt[j % 4] * strength * 0.35;
            }
            y1 = av_clipf(y1, -128, 127);
            if (flags & NOISE_AVERAGED)
                y1 /= 3.0;
            noise[i] = (int)y1;
        }
        if (RAND_N(6) == 0)
            j--;
    }
    for (i = 0; i < MAX_RES; i++)
        for (j = 0; j < 3; j++)
            fp->prev_shift[i][j] = noise + (av_lfg_get(lfg) & (MAX_SHIFT - 1));
    fp->noise = noise;
    return 0;
}
