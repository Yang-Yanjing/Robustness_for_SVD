AVAudioResampleContext *avresample_alloc_context(void)
{
    AVAudioResampleContext *avr;
    avr = av_mallocz(sizeof(*avr));
    if (!avr)
        return NULL;
    avr->av_class = &av_resample_context_class;
    av_opt_set_defaults(avr);
    return avr;
}
