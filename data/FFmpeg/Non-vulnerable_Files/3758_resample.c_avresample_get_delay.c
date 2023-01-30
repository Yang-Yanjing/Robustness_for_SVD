int avresample_get_delay(AVAudioResampleContext *avr)
{
    ResampleContext *c = avr->resample;
    if (!avr->resample_needed || !avr->resample)
        return 0;
    return FFMAX(c->buffer->nb_samples - c->padding_size, 0);
}
