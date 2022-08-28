static void yae_downmix(ATempoContext *atempo, AudioFragment *frag)
{
    
    const uint8_t *src = frag->data;
    
    memset(frag->xdat, 0, sizeof(FFTComplex) * atempo->window);
    if (atempo->format == AV_SAMPLE_FMT_U8) {
        yae_init_xdat(uint8_t, 127);
    } else if (atempo->format == AV_SAMPLE_FMT_S16) {
        yae_init_xdat(int16_t, 32767);
    } else if (atempo->format == AV_SAMPLE_FMT_S32) {
        yae_init_xdat(int, 2147483647);
    } else if (atempo->format == AV_SAMPLE_FMT_FLT) {
        yae_init_xdat(float, 1);
    } else if (atempo->format == AV_SAMPLE_FMT_DBL) {
        yae_init_xdat(double, 1);
    }
}
