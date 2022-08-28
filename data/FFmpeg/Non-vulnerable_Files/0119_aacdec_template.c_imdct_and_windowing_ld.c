static void imdct_and_windowing_ld(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *in    = sce->coeffs;
    INTFLOAT *out   = sce->ret;
    INTFLOAT *saved = sce->saved;
    INTFLOAT *buf  = ac->buf_mdct;
#if USE_FIXED
    int i;
#endif 
    
    ac->mdct.imdct_half(&ac->mdct_ld, buf, in);
#if USE_FIXED
    for (i = 0; i < 1024; i++)
        buf[i] = (buf[i] + 2) >> 2;
#endif 
    
    if (ics->use_kb_window[1]) {
        
        memcpy(out, saved, 192 * sizeof(*out));
        ac->fdsp->vector_fmul_window(out + 192, saved + 192, buf, AAC_RENAME(ff_sine_128), 64);
        memcpy(                     out + 320, buf + 64, 192 * sizeof(*out));
    } else {
        ac->fdsp->vector_fmul_window(out, saved, buf, AAC_RENAME(ff_sine_512), 256);
    }
    
    memcpy(saved, buf + 256, 256 * sizeof(*saved));
}
