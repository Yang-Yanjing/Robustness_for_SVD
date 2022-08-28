void ff_set_fixed_vector(float *out, const AMRFixed *in, float scale, int size)
{
    int i;
    for (i=0; i < in->n; i++) {
        int x   = in->x[i], repeats = !((in->no_repeat_mask >> i) & 1);
        float y = in->y[i] * scale;
        if (in->pitch_lag > 0)
            av_assert0(x < size);
            do {
                out[x] += y;
                y *= in->pitch_fac;
                x += in->pitch_lag;
            } while (x < size && repeats);
    }
}
