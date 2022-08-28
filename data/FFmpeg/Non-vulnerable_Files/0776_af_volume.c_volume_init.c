static av_cold void volume_init(VolumeContext *vol)
{
    vol->samples_align = 1;
    switch (av_get_packed_sample_fmt(vol->sample_fmt)) {
    case AV_SAMPLE_FMT_U8:
        if (vol->volume_i < 0x1000000)
            vol->scale_samples = scale_samples_u8_small;
        else
            vol->scale_samples = scale_samples_u8;
        break;
    case AV_SAMPLE_FMT_S16:
        if (vol->volume_i < 0x10000)
            vol->scale_samples = scale_samples_s16_small;
        else
            vol->scale_samples = scale_samples_s16;
        break;
    case AV_SAMPLE_FMT_S32:
        vol->scale_samples = scale_samples_s32;
        break;
    case AV_SAMPLE_FMT_FLT:
        vol->samples_align = 4;
        break;
    case AV_SAMPLE_FMT_DBL:
        vol->samples_align = 8;
        break;
    }
    if (ARCH_X86)
        ff_volume_init_x86(vol);
}
