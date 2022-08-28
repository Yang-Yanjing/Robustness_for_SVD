void ff_audio_mix_free(AudioMix **am_p)
{
    AudioMix *am;
    if (!*am_p)
        return;
    am = *am_p;
    if (am->matrix) {
        av_free(am->matrix[0]);
        am->matrix = NULL;
    }
    memset(am->matrix_q8,  0, sizeof(am->matrix_q8 ));
    memset(am->matrix_q15, 0, sizeof(am->matrix_q15));
    memset(am->matrix_flt, 0, sizeof(am->matrix_flt));
    av_freep(am_p);
}
