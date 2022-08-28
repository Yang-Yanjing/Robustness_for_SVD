static void precalculate_fade_factors(double *fade_factors[2], int frame_len)
{
    const double step_size = 1.0 / frame_len;
    int pos;
    for (pos = 0; pos < frame_len; pos++) {
        fade_factors[0][pos] = 1.0 - (step_size * (pos + 1.0));
        fade_factors[1][pos] = 1.0 - fade_factors[0][pos];
    }
}
