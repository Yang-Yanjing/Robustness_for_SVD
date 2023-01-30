/** Initializes an ALSA playback stream */
static int Start (audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    aout_sys_t *sys = aout->sys;
    snd_pcm_format_t pcm_format; /* ALSA sample format */
    bool spdif = false;
    switch (fmt->i_format)
    {
        case VLC_CODEC_FL64:
            pcm_format = SND_PCM_FORMAT_FLOAT64;
            break;
        case VLC_CODEC_FL32:
            pcm_format = SND_PCM_FORMAT_FLOAT;
            break;
        case VLC_CODEC_S32N:
            pcm_format = SND_PCM_FORMAT_S32;
            break;
        case VLC_CODEC_S16N:
            pcm_format = SND_PCM_FORMAT_S16;
            break;
        case VLC_CODEC_U8:
            pcm_format = SND_PCM_FORMAT_U8;
            break;
        default:
            if (AOUT_FMT_SPDIF(fmt))
                spdif = var_InheritBool (aout, "spdif");
            if (spdif)
            {
                fmt->i_format = VLC_CODEC_SPDIFL;
                pcm_format = SND_PCM_FORMAT_S16;
            }
            else
            if (HAVE_FPU)
            {
                fmt->i_format = VLC_CODEC_FL32;
                pcm_format = SND_PCM_FORMAT_FLOAT;
            }
            else
            {
                fmt->i_format = VLC_CODEC_S16N;
                pcm_format = SND_PCM_FORMAT_S16;
            }
    }
    const char *device = sys->device;
    /* Choose the IEC device for S/PDIF output */
    char sep = '\0';
    if (spdif)
    {
        const char *opt = NULL;
        if (!strcmp (device, "default"))
            device = "iec958"; /* TODO: hdmi */
        if (!strncmp (device, "iec958", 6))
            opt = device + 6;
        if (!strncmp (device, "hdmi", 4))
            opt = device + 4;
        if (opt != NULL)
            switch (*opt)
            {
                case ':':  sep = ','; break;
                case '\0': sep = ':'; break;
            }
    }
    char *devbuf = NULL;
    if (sep != '\0')
    {
        unsigned aes3;
        switch (fmt->i_rate)
        {
#define FS(freq) \
            case freq: aes3 = IEC958_AES3_CON_FS_ ## freq; break;
            FS( 44100) /* def. */ FS( 48000) FS( 32000)
            FS( 22050)            FS( 24000)
            FS( 88200) FS(768000) FS( 96000)
            FS(176400)            FS(192000)
#undef FS
            default:
                aes3 = IEC958_AES3_CON_FS_NOTID;
                break;
        }
        if (asprintf (&devbuf, "%s%cAES0=0x%x,AES1=0x%x,AES2=0x%x,AES3=0x%x",
                      device, sep,
                      IEC958_AES0_CON_EMPHASIS_NONE | IEC958_AES0_NONAUDIO,
                      IEC958_AES1_CON_ORIGINAL | IEC958_AES1_CON_PCM_CODER,
                      0, aes3) == -1)
            return VLC_ENOMEM;
        device = devbuf;
    }
    /* Open the device */
    snd_pcm_t *pcm;
    /* VLC always has a resampler. No need for ALSA's. */
    const int mode = SND_PCM_NO_AUTO_RESAMPLE;
    int val = snd_pcm_open (&pcm, device, SND_PCM_STREAM_PLAYBACK, mode);
    if (val != 0)
    {
        msg_Err (aout, "cannot open ALSA device \"%s\": %s", device,
                 snd_strerror (val));
        dialog_Fatal (aout, _("Audio output failed"),
                      _("The audio device \"%s\" could not be used:\n%s."),
                      sys->device, snd_strerror (val));
        free (devbuf);
        return VLC_EGENERIC;
    }
    sys->pcm = pcm;
    /* Print some potentially useful debug */
    msg_Dbg (aout, "using ALSA device: %s", device);
    free (devbuf);
    DumpDevice (VLC_OBJECT(aout), pcm);
    /* Get Initial hardware parameters */
    snd_pcm_hw_params_t *hw;
    unsigned param;
    snd_pcm_hw_params_alloca (&hw);
    snd_pcm_hw_params_any (pcm, hw);
    Dump (aout, "initial hardware setup:\n", snd_pcm_hw_params_dump, hw);
    val = snd_pcm_hw_params_set_rate_resample(pcm, hw, 0);
    if (val)
    {
        msg_Err (aout, "cannot disable resampling: %s", snd_strerror (val));
        goto error;
    }
    val = snd_pcm_hw_params_set_access (pcm, hw,
                                        SND_PCM_ACCESS_RW_INTERLEAVED);
    if (val)
    {
        msg_Err (aout, "cannot set access mode: %s", snd_strerror (val));
        goto error;
    }
    /* Set sample format */
    if (snd_pcm_hw_params_test_format (pcm, hw, pcm_format) == 0)
        ;
    else
    if (snd_pcm_hw_params_test_format (pcm, hw, SND_PCM_FORMAT_FLOAT) == 0)
    {
        fmt->i_format = VLC_CODEC_FL32;
        pcm_format = SND_PCM_FORMAT_FLOAT;
    }
    else
    if (snd_pcm_hw_params_test_format (pcm, hw, SND_PCM_FORMAT_S32) == 0)
    {
        fmt->i_format = VLC_CODEC_S32N;
        pcm_format = SND_PCM_FORMAT_S32;
    }
    else
    if (snd_pcm_hw_params_test_format (pcm, hw, SND_PCM_FORMAT_S16) == 0)
    {
        fmt->i_format = VLC_CODEC_S16N;
        pcm_format = SND_PCM_FORMAT_S16;
    }
    else
    {
        msg_Err (aout, "no supported sample format");
        goto error;
    }
    val = snd_pcm_hw_params_set_format (pcm, hw, pcm_format);
    if (val)
    {
        msg_Err (aout, "cannot set sample format: %s", snd_strerror (val));
        goto error;
    }
    /* Set channels count */
    unsigned channels;
    if (!spdif)
    {
        uint16_t map = var_InheritInteger (aout, "alsa-audio-channels");
        sys->chans_to_reorder = SetupChannels (VLC_OBJECT(aout), pcm, &map,
                                               sys->chans_table);
        fmt->i_physical_channels = map;
        fmt->i_original_channels = map;
        channels = popcount (map);
    }
    else
    {
        sys->chans_to_reorder = 0;
        channels = 2;
    }
    /* By default, ALSA plug will pad missing channels with zeroes, which is
     * usually fine. However, it will also discard extraneous channels, which
     * is not acceptable. Thus the user must configure the physically
     * available channels, and VLC will downmix if needed. */
    val = snd_pcm_hw_params_set_channels (pcm, hw, channels);
    if (val)
    {
        msg_Err (aout, "cannot set %u channels: %s", channels,
                 snd_strerror (val));
        goto error;
    }
    /* Set sample rate */
    val = snd_pcm_hw_params_set_rate_near (pcm, hw, &fmt->i_rate, NULL);
    if (val)
    {
        msg_Err (aout, "cannot set sample rate: %s", snd_strerror (val));
        goto error;
    }
    sys->rate = fmt->i_rate;
    /* Set buffer size */
    param = AOUT_MAX_ADVANCE_TIME;
    val = snd_pcm_hw_params_set_buffer_time_near (pcm, hw, &param, NULL);
    if (val)
    {
        msg_Err (aout, "cannot set buffer duration: %s", snd_strerror (val));
        goto error;
    }
    param = AOUT_MIN_PREPARE_TIME;
    val = snd_pcm_hw_params_set_period_time_near (pcm, hw, &param, NULL);
    if (val)
    {
        msg_Err (aout, "cannot set period: %s", snd_strerror (val));
        goto error;
    }
    /* Commit hardware parameters */
    val = snd_pcm_hw_params (pcm, hw);
    if (val < 0)
    {
        msg_Err (aout, "cannot commit hardware parameters: %s",
                 snd_strerror (val));
        goto error;
    }
    Dump (aout, "final HW setup:\n", snd_pcm_hw_params_dump, hw);
    /* Get Initial software parameters */
    snd_pcm_sw_params_t *sw;
    snd_pcm_sw_params_alloca (&sw);
    snd_pcm_sw_params_current (pcm, sw);
    Dump (aout, "initial software parameters:\n", snd_pcm_sw_params_dump, sw);
    /* START REVISIT */
    //snd_pcm_sw_params_set_avail_min( pcm, sw, i_period_size );
    // FIXME: useful?
    val = snd_pcm_sw_params_set_start_threshold (pcm, sw, 1);
    if( val < 0 )
    {
        msg_Err( aout, "unable to set start threshold (%s)",
                 snd_strerror( val ) );
        goto error;
    }
    /* END REVISIT */
    /* Commit software parameters. */
    val = snd_pcm_sw_params (pcm, sw);
    if (val)
    {
        msg_Err (aout, "cannot commit software parameters: %s",
                 snd_strerror (val));
        goto error;
    }
    Dump (aout, "final software parameters:\n", snd_pcm_sw_params_dump, sw);
    val = snd_pcm_prepare (pcm);
    if (val)
    {
        msg_Err (aout, "cannot prepare device: %s", snd_strerror (val));
        goto error;
    }
    /* Setup audio_output_t */
    if (spdif)
    {
        fmt->i_bytes_per_frame = AOUT_SPDIF_SIZE;
        fmt->i_frame_length = A52_FRAME_NB;
    }
    sys->format = fmt->i_format;
    aout->time_get = TimeGet;
    aout->play = Play;
    if (snd_pcm_hw_params_can_pause (hw))
        aout->pause = Pause;
    else
    {
        aout->pause = PauseDummy;
        msg_Warn (aout, "device cannot be paused");
    }
    aout->flush = Flush;
    aout_SoftVolumeStart (aout);
    return 0;
error:
    snd_pcm_close (pcm);
    return VLC_EGENERIC;
}
