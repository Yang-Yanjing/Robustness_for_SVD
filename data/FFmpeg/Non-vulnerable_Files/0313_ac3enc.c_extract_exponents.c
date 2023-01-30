static void extract_exponents(AC3EncodeContext *s)
{
    int ch        = !s->cpl_on;
    int chan_size = AC3_MAX_COEFS * s->num_blocks * (s->channels - ch + 1);
    AC3Block *block = &s->blocks[0];
    s->ac3dsp.extract_exponents(block->exp[ch], block->fixed_coef[ch], chan_size);
}
