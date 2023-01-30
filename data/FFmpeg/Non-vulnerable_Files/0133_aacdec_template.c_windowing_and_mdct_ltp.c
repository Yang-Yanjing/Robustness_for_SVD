static void windowing_and_mdct_ltp(AACContext *ac, INTFLOAT *out,
                                   INTFLOAT *in, IndividualChannelStream *ics)
{
    const INTFLOAT *lwindow      = ics->use_kb_window[0] ? AAC_RENAME(ff_aac_kbd_long_1024) : AAC_RENAME(ff_sine_1024);
    const INTFLOAT *swindow      = ics->use_kb_window[0] ? AAC_RENAME(ff_aac_kbd_short_128) : AAC_RENAME(ff_sine_128);
    const INTFLOAT *lwindow_prev = ics->use_kb_window[1] ? AAC_RENAME(ff_aac_kbd_long_1024) : AAC_RENAME(ff_sine_1024);
    const INTFLOAT *swindow_prev = ics->use_kb_window[1] ? AAC_RENAME(ff_aac_kbd_short_128) : AAC_RENAME(ff_sine_128);
    if (ics->window_sequence[0] != LONG_STOP_SEQUENCE) {
        ac->fdsp->vector_fmul(in, in, lwindow_prev, 1024);
    } else {
        memset(in, 0, 448 * sizeof(*in));
        ac->fdsp->vector_fmul(in + 448, in + 448, swindow_prev, 128);
    }
    if (ics->window_sequence[0] != LONG_START_SEQUENCE) {
        ac->fdsp->vector_fmul_reverse(in + 1024, in + 1024, lwindow, 1024);
    } else {
        ac->fdsp->vector_fmul_reverse(in + 1024 + 448, in + 1024 + 448, swindow, 128);
        memset(in + 1024 + 576, 0, 448 * sizeof(*in));
    }
    ac->mdct_ltp.mdct_calc(&ac->mdct_ltp, out, in);
}
