DitherContext *ff_dither_alloc(AVAudioResampleContext *avr,
                               enum AVSampleFormat out_fmt,
                               enum AVSampleFormat in_fmt,
                               int channels, int sample_rate, int apply_map)
{
    AVLFG seed_gen;
    DitherContext *c;
    int ch;
    if (av_get_packed_sample_fmt(out_fmt) != AV_SAMPLE_FMT_S16 ||
        av_get_bytes_per_sample(in_fmt) <= 2) {
        av_log(avr, AV_LOG_ERROR, "dithering %s to %s is not supported\n",
               av_get_sample_fmt_name(in_fmt), av_get_sample_fmt_name(out_fmt));
        return NULL;
    }
    c = av_mallocz(sizeof(*c));
    if (!c)
        return NULL;
    c->apply_map = apply_map;
    if (apply_map)
        c->ch_map_info = &avr->ch_map_info;
    if (avr->dither_method == AV_RESAMPLE_DITHER_TRIANGULAR_NS &&
        sample_rate != 48000 && sample_rate != 44100) {
        av_log(avr, AV_LOG_WARNING, "sample rate must be 48000 or 44100 Hz "
               "for triangular_ns dither. using triangular_hp instead.\n");
        avr->dither_method = AV_RESAMPLE_DITHER_TRIANGULAR_HP;
    }
    c->method = avr->dither_method;
    dither_init(&c->ddsp, c->method);
    if (c->method == AV_RESAMPLE_DITHER_TRIANGULAR_NS) {
        if (sample_rate == 48000) {
            c->ns_coef_b = ns_48_coef_b;
            c->ns_coef_a = ns_48_coef_a;
        } else {
            c->ns_coef_b = ns_44_coef_b;
            c->ns_coef_a = ns_44_coef_a;
        }
    }
    


    if (out_fmt != AV_SAMPLE_FMT_S16P) {
        c->s16_data = ff_audio_data_alloc(channels, 1024, AV_SAMPLE_FMT_S16P,
                                          "dither s16 buffer");
        if (!c->s16_data)
            goto fail;
        c->ac_out = ff_audio_convert_alloc(avr, out_fmt, AV_SAMPLE_FMT_S16P,
                                           channels, sample_rate, 0);
        if (!c->ac_out)
            goto fail;
    }
    if (in_fmt != AV_SAMPLE_FMT_FLTP || c->apply_map) {
        c->flt_data = ff_audio_data_alloc(channels, 1024, AV_SAMPLE_FMT_FLTP,
                                          "dither flt buffer");
        if (!c->flt_data)
            goto fail;
    }
    if (in_fmt != AV_SAMPLE_FMT_FLTP) {
        c->ac_in = ff_audio_convert_alloc(avr, AV_SAMPLE_FMT_FLTP, in_fmt,
                                          channels, sample_rate, c->apply_map);
        if (!c->ac_in)
            goto fail;
    }
    c->state = av_mallocz(channels * sizeof(*c->state));
    if (!c->state)
        goto fail;
    c->channels = channels;
    

    c->mute_dither_threshold = lrintf(sample_rate * MUTE_THRESHOLD_SEC);
    c->mute_reset_threshold  = c->mute_dither_threshold * 4;
    
    av_lfg_init(&seed_gen, 0xC0FFEE);
    for (ch = 0; ch < channels; ch++) {
        DitherState *state = &c->state[ch];
        state->mute = c->mute_reset_threshold + 1;
        state->seed = av_lfg_get(&seed_gen);
        generate_dither_noise(c, state, FFMAX(32768, sample_rate / 2));
    }
    return c;
fail:
    ff_dither_free(&c);
    return NULL;
}
