void ff_dither_free(DitherContext **cp)
{
    DitherContext *c = *cp;
    int ch;
    if (!c)
        return;
    ff_audio_data_free(&c->flt_data);
    ff_audio_data_free(&c->s16_data);
    ff_audio_convert_free(&c->ac_in);
    ff_audio_convert_free(&c->ac_out);
    for (ch = 0; ch < c->channels; ch++)
        av_free(c->state[ch].noise_buf);
    av_free(c->state);
    av_freep(cp);
}
