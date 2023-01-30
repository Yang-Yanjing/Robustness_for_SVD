}
static int Start (audio_output_t *aout, audio_sample_format_t *fmt)
{
    aout_sys_t *sys = aout->sys;
    char format[5] = "S16N";
    unsigned channels;
    if (sys->setup != NULL)
    {
        channels = aout_FormatNbChannels(fmt);
        sys->opaque = sys->setup_opaque;
        if (sys->setup (&sys->opaque, format, &fmt->i_rate, &channels))
            return VLC_EGENERIC;
    }
    else
    {
        fmt->i_rate = sys->rate;
        channels = sys->channels;
    }
    /* Initialize volume (in case the UI changed volume before setup) */
    sys->ready = true;
    if (sys->set_volume != NULL)
        sys->set_volume(sys->opaque, sys->volume, sys->mute);
    /* Ensure that format is supported */
    if (fmt->i_rate == 0 || fmt->i_rate > 192000
     || channels == 0 || channels > AOUT_CHAN_MAX
     || strcmp(format, "S16N") /* TODO: amem-format */)
    {
        msg_Err (aout, "format not supported: %s, %u channel(s), %u Hz",
                 format, channels, fmt->i_rate);
        Stop (aout);
        return VLC_EGENERIC;
    }
    /* channel mapping */
    switch (channels)
    {
        case 1:
            fmt->i_physical_channels = AOUT_CHAN_CENTER;
            break;
        case 2:
            fmt->i_physical_channels = AOUT_CHANS_2_0;
            break;
        case 3:
            fmt->i_physical_channels = AOUT_CHANS_2_1;
            break;
        case 4:
            fmt->i_physical_channels = AOUT_CHANS_4_0;
            break;
        case 5:
            fmt->i_physical_channels = AOUT_CHANS_5_0;
            break;
        case 6:
            fmt->i_physical_channels = AOUT_CHANS_5_1;
            break;
        case 7:
            fmt->i_physical_channels =
                AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT | AOUT_CHAN_CENTER |
                AOUT_CHAN_REARCENTER | AOUT_CHAN_MIDDLELEFT |
                AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_LFE;
            break;
        case 8:
            fmt->i_physical_channels = AOUT_CHANS_7_1;
            break;
        default:
            assert(0);
    }
    fmt->i_format = VLC_CODEC_S16N;
    fmt->i_original_channels = fmt->i_physical_channels;
    return VLC_SUCCESS;
}
