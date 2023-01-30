static av_cold int mix_function_init(AudioMix *am)
{
    am->func_descr = am->func_descr_generic = "n/a";
    am->mix = am->mix_generic = NULL;
    
    if (!am->in_matrix_channels || !am->out_matrix_channels)
        return 0;
    
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_FLTP, AV_MIX_COEFF_TYPE_FLT,
                          0, 0, 1, 1, "C", MIX_FUNC_NAME(FLTP, FLT));
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_S16P, AV_MIX_COEFF_TYPE_FLT,
                          0, 0, 1, 1, "C", MIX_FUNC_NAME(S16P, FLT));
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_S16P, AV_MIX_COEFF_TYPE_Q15,
                          0, 0, 1, 1, "C", MIX_FUNC_NAME(S16P, Q15));
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_S16P, AV_MIX_COEFF_TYPE_Q8,
                          0, 0, 1, 1, "C", MIX_FUNC_NAME(S16P, Q8));
    
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_FLTP, AV_MIX_COEFF_TYPE_FLT,
                          2, 1, 1, 1, "C", mix_2_to_1_fltp_flt_c);
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_S16P, AV_MIX_COEFF_TYPE_FLT,
                          2, 1, 1, 1, "C", mix_2_to_1_s16p_flt_c);
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_S16P, AV_MIX_COEFF_TYPE_Q8,
                          2, 1, 1, 1, "C", mix_2_to_1_s16p_q8_c);
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_FLTP, AV_MIX_COEFF_TYPE_FLT,
                          1, 2, 1, 1, "C", mix_1_to_2_fltp_flt_c);
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_FLTP, AV_MIX_COEFF_TYPE_FLT,
                          6, 2, 1, 1, "C", mix_6_to_2_fltp_flt_c);
    ff_audio_mix_set_func(am, AV_SAMPLE_FMT_FLTP, AV_MIX_COEFF_TYPE_FLT,
                          2, 6, 1, 1, "C", mix_2_to_6_fltp_flt_c);
    if (ARCH_X86)
        ff_audio_mix_init_x86(am);
    if (!am->mix) {
        av_log(am->avr, AV_LOG_ERROR, "audio_mix: NO FUNCTION FOUND: [fmt=%s] "
               "[c=%s] [%d to %d]\n", av_get_sample_fmt_name(am->fmt),
               coeff_type_names[am->coeff_type], am->in_channels,
               am->out_channels);
        return AVERROR_PATCHWELCOME;
    }
    return 0;
}
