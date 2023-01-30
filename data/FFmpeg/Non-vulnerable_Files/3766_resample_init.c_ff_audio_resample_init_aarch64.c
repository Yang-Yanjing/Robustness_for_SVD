                              unsigned int index, int frac);
av_cold void ff_audio_resample_init_aarch64(ResampleContext *c,
                                            enum AVSampleFormat sample_fmt)
{
    int cpu_flags = av_get_cpu_flags();
    if (have_neon(cpu_flags)) {
        if (!c->linear) {
            switch (sample_fmt) {
            case AV_SAMPLE_FMT_DBLP:
                c->resample_one  = ff_resample_one_dbl_neon;
                break;
            case AV_SAMPLE_FMT_FLTP:
                c->resample_one  = ff_resample_one_flt_neon;
                break;
            case AV_SAMPLE_FMT_S16P:
                c->resample_one  = ff_resample_one_s16_neon;
                break;
            case AV_SAMPLE_FMT_S32P:
                c->resample_one  = ff_resample_one_s32_neon;
                break;
            }
        }
    }
}
