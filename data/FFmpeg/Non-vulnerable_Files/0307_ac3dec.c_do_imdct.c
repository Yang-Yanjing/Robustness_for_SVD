static inline void do_imdct(AC3DecodeContext *s, int channels)
{
    int ch;
    for (ch = 1; ch <= channels; ch++) {
        if (s->block_switch[ch]) {
            int i;
            FFTSample *x = s->tmp_output + 128;
            for (i = 0; i < 128; i++)
                x[i] = s->transform_coeffs[ch][2 * i];
            s->imdct_256.imdct_half(&s->imdct_256, s->tmp_output, x);
#if USE_FIXED
            s->fdsp->vector_fmul_window_scaled(s->outptr[ch - 1], s->delay[ch - 1],
                                       s->tmp_output, s->window, 128, 8);
#else
            s->fdsp->vector_fmul_window(s->outptr[ch - 1], s->delay[ch - 1],
                                       s->tmp_output, s->window, 128);
#endif
            for (i = 0; i < 128; i++)
                x[i] = s->transform_coeffs[ch][2 * i + 1];
            s->imdct_256.imdct_half(&s->imdct_256, s->delay[ch - 1], x);
        } else {
            s->imdct_512.imdct_half(&s->imdct_512, s->tmp_output, s->transform_coeffs[ch]);
#if USE_FIXED
            s->fdsp->vector_fmul_window_scaled(s->outptr[ch - 1], s->delay[ch - 1],
                                       s->tmp_output, s->window, 128, 8);
#else
            s->fdsp->vector_fmul_window(s->outptr[ch - 1], s->delay[ch - 1],
                                       s->tmp_output, s->window, 128);
#endif
            memcpy(s->delay[ch - 1], s->tmp_output + 128, 128 * sizeof(FFTSample));
        }
    }
}
