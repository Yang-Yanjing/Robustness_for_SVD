static void apply_mdct(AC3EncodeContext *s)
{
    int blk, ch;
    for (ch = 0; ch < s->channels; ch++) {
        for (blk = 0; blk < s->num_blocks; blk++) {
            AC3Block *block = &s->blocks[blk];
            const SampleType *input_samples = &s->planar_samples[ch][blk * AC3_BLOCK_SIZE];
#if CONFIG_AC3ENC_FLOAT
            s->fdsp->vector_fmul(s->windowed_samples, input_samples,
                                s->mdct_window, AC3_WINDOW_SIZE);
#else
            s->ac3dsp.apply_window_int16(s->windowed_samples, input_samples,
                                         s->mdct_window, AC3_WINDOW_SIZE);
#endif
            if (s->fixed_point)
                block->coeff_shift[ch+1] = normalize_samples(s);
            s->mdct.mdct_calcw(&s->mdct, block->mdct_coef[ch+1],
                               s->windowed_samples);
        }
    }
}
