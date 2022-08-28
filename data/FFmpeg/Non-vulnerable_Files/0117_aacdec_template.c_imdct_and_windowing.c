static void imdct_and_windowing(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *in    = sce->coeffs;
    INTFLOAT *out   = sce->ret;
    INTFLOAT *saved = sce->saved;
    const INTFLOAT *swindow      = ics->use_kb_window[0] ? AAC_RENAME(ff_aac_kbd_short_128) : AAC_RENAME(ff_sine_128);
    const INTFLOAT *lwindow_prev = ics->use_kb_window[1] ? AAC_RENAME(ff_aac_kbd_long_1024) : AAC_RENAME(ff_sine_1024);
    const INTFLOAT *swindow_prev = ics->use_kb_window[1] ? AAC_RENAME(ff_aac_kbd_short_128) : AAC_RENAME(ff_sine_128);
    INTFLOAT *buf  = ac->buf_mdct;
    INTFLOAT *temp = ac->temp;
    int i;
    
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        for (i = 0; i < 1024; i += 128)
            ac->mdct_small.imdct_half(&ac->mdct_small, buf + i, in + i);
    } else {
        ac->mdct.imdct_half(&ac->mdct, buf, in);
#if USE_FIXED
        for (i=0; i<1024; i++)
          buf[i] = (buf[i] + 4) >> 3;
#endif 
    }
    





    if ((ics->window_sequence[1] == ONLY_LONG_SEQUENCE || ics->window_sequence[1] == LONG_STOP_SEQUENCE) &&
            (ics->window_sequence[0] == ONLY_LONG_SEQUENCE || ics->window_sequence[0] == LONG_START_SEQUENCE)) {
        ac->fdsp->vector_fmul_window(    out,               saved,            buf,         lwindow_prev, 512);
    } else {
        memcpy(                         out,               saved,            448 * sizeof(*out));
        if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
            ac->fdsp->vector_fmul_window(out + 448 + 0*128, saved + 448,      buf + 0*128, swindow_prev, 64);
            ac->fdsp->vector_fmul_window(out + 448 + 1*128, buf + 0*128 + 64, buf + 1*128, swindow,      64);
            ac->fdsp->vector_fmul_window(out + 448 + 2*128, buf + 1*128 + 64, buf + 2*128, swindow,      64);
            ac->fdsp->vector_fmul_window(out + 448 + 3*128, buf + 2*128 + 64, buf + 3*128, swindow,      64);
            ac->fdsp->vector_fmul_window(temp,              buf + 3*128 + 64, buf + 4*128, swindow,      64);
            memcpy(                     out + 448 + 4*128, temp, 64 * sizeof(*out));
        } else {
            ac->fdsp->vector_fmul_window(out + 448,         saved + 448,      buf,         swindow_prev, 64);
            memcpy(                     out + 576,         buf + 64,         448 * sizeof(*out));
        }
    }
    
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy(                     saved,       temp + 64,         64 * sizeof(*saved));
        ac->fdsp->vector_fmul_window(saved + 64,  buf + 4*128 + 64, buf + 5*128, swindow, 64);
        ac->fdsp->vector_fmul_window(saved + 192, buf + 5*128 + 64, buf + 6*128, swindow, 64);
        ac->fdsp->vector_fmul_window(saved + 320, buf + 6*128 + 64, buf + 7*128, swindow, 64);
        memcpy(                     saved + 448, buf + 7*128 + 64,  64 * sizeof(*saved));
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy(                     saved,       buf + 512,        448 * sizeof(*saved));
        memcpy(                     saved + 448, buf + 7*128 + 64,  64 * sizeof(*saved));
    } else { 
        memcpy(                     saved,       buf + 512,        512 * sizeof(*saved));
    }
}
