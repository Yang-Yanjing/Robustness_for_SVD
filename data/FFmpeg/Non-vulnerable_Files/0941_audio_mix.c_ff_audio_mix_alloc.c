AudioMix *ff_audio_mix_alloc(AVAudioResampleContext *avr)
{
    AudioMix *am;
    int ret;
    am = av_mallocz(sizeof(*am));
    if (!am)
        return NULL;
    am->avr = avr;
    if (avr->internal_sample_fmt != AV_SAMPLE_FMT_S16P &&
        avr->internal_sample_fmt != AV_SAMPLE_FMT_FLTP) {
        av_log(avr, AV_LOG_ERROR, "Unsupported internal format for "
               "mixing: %s\n",
               av_get_sample_fmt_name(avr->internal_sample_fmt));
        goto error;
    }
    am->fmt          = avr->internal_sample_fmt;
    am->coeff_type   = avr->mix_coeff_type;
    am->in_layout    = avr->in_channel_layout;
    am->out_layout   = avr->out_channel_layout;
    am->in_channels  = avr->in_channels;
    am->out_channels = avr->out_channels;
    
    if (avr->mix_matrix) {
        ret = ff_audio_mix_set_matrix(am, avr->mix_matrix, avr->in_channels);
        if (ret < 0)
            goto error;
        av_freep(&avr->mix_matrix);
    } else {
        double *matrix_dbl = av_mallocz(avr->out_channels * avr->in_channels *
                                        sizeof(*matrix_dbl));
        if (!matrix_dbl)
            goto error;
        ret = avresample_build_matrix(avr->in_channel_layout,
                                      avr->out_channel_layout,
                                      avr->center_mix_level,
                                      avr->surround_mix_level,
                                      avr->lfe_mix_level,
                                      avr->normalize_mix_level,
                                      matrix_dbl,
                                      avr->in_channels,
                                      avr->matrix_encoding);
        if (ret < 0) {
            av_free(matrix_dbl);
            goto error;
        }
        ret = ff_audio_mix_set_matrix(am, matrix_dbl, avr->in_channels);
        if (ret < 0) {
            av_log(avr, AV_LOG_ERROR, "error setting mix matrix\n");
            av_free(matrix_dbl);
            goto error;
        }
        av_free(matrix_dbl);
    }
    return am;
error:
    av_free(am);
    return NULL;
}
