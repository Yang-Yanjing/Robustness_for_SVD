static void Flush (audio_output_t *, bool);
static int Start (audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    aout_sys_t* sys = aout->sys;
    /* Open the device */
    const char *device = sys->device;
    if (device == NULL)
        device = getenv ("OSS_AUDIODEV");
    if (device == NULL)
        device = "/dev/dsp";
    int fd = vlc_open (device, O_WRONLY);
    if (fd == -1)
    {
        msg_Err (aout, "cannot open OSS device %s: %s", device,
                 vlc_strerror_c(errno));
        return VLC_EGENERIC;
    }
    msg_Dbg (aout, "using OSS device: %s", device);
    /* Select audio format */
    int format;
    bool spdif = false;
    switch (fmt->i_format)
    {
#ifdef AFMT_FLOAT
        case VLC_CODEC_FL64:
        case VLC_CODEC_FL32:
            format = AFMT_FLOAT;
            break;
#endif
        case VLC_CODEC_S32N:
            format = AFMT_S32_NE;
            break;
        case VLC_CODEC_S16N:
            format = AFMT_S16_NE;
            break;
        case VLC_CODEC_U8:
            format = AFMT_U8;
            break;
        default:
            if (AOUT_FMT_SPDIF(fmt))
                spdif = var_InheritBool (aout, "spdif");
            if (spdif)
                format = AFMT_AC3;
#ifdef AFMT_FLOAT
            else if (HAVE_FPU)
                format = AFMT_FLOAT;
#endif
            else
                format = AFMT_S16_NE;
    }
    if (ioctl (fd, SNDCTL_DSP_SETFMT, &format) < 0)
    {
        msg_Err (aout, "cannot set audio format 0x%X: %s", format,
                 vlc_strerror_c(errno));
        goto error;
    }
    switch (format)
    {
        case AFMT_U8:     fmt->i_format = VLC_CODEC_U8;   break;
        case AFMT_S16_NE: fmt->i_format = VLC_CODEC_S16N; break;
        case AFMT_S32_NE: fmt->i_format = VLC_CODEC_S32N; break;
#ifdef AFMT_FLOAT
        case AFMT_FLOAT:  fmt->i_format = VLC_CODEC_FL32; break;
#endif
        case AFMT_AC3:
            if (spdif)
            {
                fmt->i_format = VLC_CODEC_SPDIFL;
                break;
            }
        default:
            msg_Err (aout, "unsupported audio format 0x%X", format);
            goto error;
    }
    /* Select channels count */
    int channels = spdif ? 2 : aout_FormatNbChannels (fmt);
    if (ioctl (fd, SNDCTL_DSP_CHANNELS, &channels) < 0)
    {
        msg_Err (aout, "cannot set %d channels: %s", channels,
                 vlc_strerror_c(errno));
        goto error;
    }
    switch (channels)
    {
        case 1: channels = AOUT_CHAN_CENTER;  break;
        case 2: channels = AOUT_CHANS_STEREO; break;
        case 4: channels = AOUT_CHANS_4_0;    break;
        case 6: channels = AOUT_CHANS_5_1;    break;
        case 8: channels = AOUT_CHANS_7_1;    break;
        default:
            msg_Err (aout, "unsupported channels count %d", channels);
            goto error;
    }
    /* Select sample rate */
    int rate = spdif ? 48000 : fmt->i_rate;
    if (ioctl (fd, SNDCTL_DSP_SPEED, &rate) < 0)
    {
        msg_Err (aout, "cannot set %d Hz sample rate: %s", rate,
                 vlc_strerror_c(errno));
        goto error;
    }
    /* Setup audio_output_t */
    aout->time_get = TimeGet;
    aout->play = Play;
    aout->pause = Pause;
    aout->flush = Flush;
    if (spdif)
    {
        fmt->i_bytes_per_frame = AOUT_SPDIF_SIZE;
        fmt->i_frame_length = A52_FRAME_NB;
    }
    else
    {
        fmt->i_rate = rate;
        fmt->i_original_channels =
        fmt->i_physical_channels = channels;
    }
    aout_FormatPrepare (fmt);
    /* Select timing */
    unsigned bytes;
    if (spdif)
        bytes = AOUT_SPDIF_SIZE;
    else
        bytes = fmt->i_rate / (CLOCK_FREQ / AOUT_MIN_PREPARE_TIME)
                * fmt->i_bytes_per_frame;
    if (unlikely(bytes < 16))
        bytes = 16;
    int frag = (AOUT_MAX_ADVANCE_TIME / AOUT_MIN_PREPARE_TIME) << 16
             | (32 - clz32(bytes - 1));
    if (ioctl (fd, SNDCTL_DSP_SETFRAGMENT, &frag) < 0)
        msg_Err (aout, "cannot set 0x%08x fragment: %s", frag,
                 vlc_strerror_c(errno));
    sys->fd = fd;
    aout_SoftVolumeStart (aout);
    sys->starting = true;
    sys->format = *fmt;
    return VLC_SUCCESS;
error:
    close (fd);
    return VLC_EGENERIC;
}
