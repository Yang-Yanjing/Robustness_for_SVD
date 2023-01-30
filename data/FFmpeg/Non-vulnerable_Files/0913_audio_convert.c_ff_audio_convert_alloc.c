AudioConvert *ff_audio_convert_alloc(AVAudioResampleContext *avr,
                                     enum AVSampleFormat out_fmt,
                                     enum AVSampleFormat in_fmt,
                                     int channels, int sample_rate,
                                     int apply_map)
{
    AudioConvert *ac;
    int in_planar, out_planar;
    ac = av_mallocz(sizeof(*ac));
    if (!ac)
        return NULL;
    ac->avr      = avr;
    ac->out_fmt  = out_fmt;
    ac->in_fmt   = in_fmt;
    ac->channels = channels;
    ac->apply_map = apply_map;
    if (avr->dither_method != AV_RESAMPLE_DITHER_NONE          &&
        av_get_packed_sample_fmt(out_fmt) == AV_SAMPLE_FMT_S16 &&
        av_get_bytes_per_sample(in_fmt) > 2) {
        ac->dc = ff_dither_alloc(avr, out_fmt, in_fmt, channels, sample_rate,
                                 apply_map);
        if (!ac->dc) {
            av_free(ac);
            return NULL;
        }
        return ac;
    }
    in_planar  = ff_sample_fmt_is_planar(in_fmt, channels);
    out_planar = ff_sample_fmt_is_planar(out_fmt, channels);
    if (in_planar == out_planar) {
        ac->func_type = CONV_FUNC_TYPE_FLAT;
        ac->planes    = in_planar ? ac->channels : 1;
    } else if (in_planar)
        ac->func_type = CONV_FUNC_TYPE_INTERLEAVE;
    else
        ac->func_type = CONV_FUNC_TYPE_DEINTERLEAVE;
    set_generic_function(ac);
    if (ARCH_AARCH64)
        ff_audio_convert_init_aarch64(ac);
    if (ARCH_ARM)
        ff_audio_convert_init_arm(ac);
    if (ARCH_X86)
        ff_audio_convert_init_x86(ac);
    return ac;
}
