/** Initializes an sndio playback stream */
static int Start (audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    aout_sys_t *sys = aout->sys;
    sys->hdl = sio_open (NULL, SIO_PLAY, 0 /* blocking */);
    if (sys->hdl == NULL)
    {
        msg_Err (aout, "cannot create audio playback stream");
        return VLC_EGENERIC;
    }
    struct sio_par par;
    sio_initpar (&par);
    switch (fmt->i_format) {
    case VLC_CODEC_U8:
	par.bits = 8;
	par.sig = 0;
	break;
    case VLC_CODEC_S16N:
	par.bits = 16;
	par.sig = 1;
	par.le = SIO_LE_NATIVE;
	break;
    case VLC_CODEC_S32N:
    case VLC_CODEC_FL32:
    case VLC_CODEC_FL64:
	par.bits = 32;
	par.sig = 1;
	par.le = SIO_LE_NATIVE;
	break;
    default:
	/* use a common audio format */
	par.bits = 16;
	par.sig = 1;
	par.le = SIO_LE_NATIVE;
    }
    par.pchan = aout_FormatNbChannels (fmt);
    par.rate = fmt->i_rate;
    par.round = par.rate / 50;
    par.appbufsz = par.rate / 4;
    if (!sio_setpar (sys->hdl, &par) || !sio_getpar (sys->hdl, &par))
    {
        msg_Err (aout, "cannot negotiate audio playback parameters");
        goto error;
    }
    if (par.bps != par.bits >> 3 && !par.msb)
    {
        msg_Err (aout, "unsupported audio sample format (%u bits in %u bytes)",
                 par.bits, par.bps);
        goto error;
    }
    if (par.sig != (par.bits != 8))
    {
        msg_Err (aout, "unsupported audio sample format (%ssigned)",
                 par.sig ? "" : "un");
        goto error;
    }
    if (par.bps > 1 && par.le != SIO_LE_NATIVE)
    {
        msg_Err (aout, "unsupported audio sample format (%s endian)",
		 par.le ? "little" : "big");
        goto error;
    }
    switch (par.bits)
    {
        case 8:
            fmt->i_format = VLC_CODEC_U8;
            break;
        case 16:
            fmt->i_format = VLC_CODEC_S16N;
            break;
        case 32:
            fmt->i_format = VLC_CODEC_S32N;
            break;
        default:
            msg_Err (aout, "unsupported audio sample format (%u bits)",
                     par.bits);
            goto error;
    }
    fmt->i_rate = par.rate;
    sys->rate = par.rate;
    /* Channel map */
    unsigned chans;
    switch (par.pchan)
    {
        case 1:
            chans = AOUT_CHAN_CENTER;
            break;
        case 2:
            chans = AOUT_CHANS_STEREO;
            break;
        case 4:
            chans = AOUT_CHANS_4_0;
            break;
        case 6:
            chans = AOUT_CHANS_5_1;
            break;
        case 8:
            chans = AOUT_CHANS_7_1;
            break;
        default:
            msg_Err (aout, "unknown %u channels map", par.pchan);
            goto error;
    }
    fmt->i_original_channels = fmt->i_physical_channels = chans;
    aout_FormatPrepare (fmt);
    aout->time_get = TimeGet;
    aout->play = Play;
    aout->pause = NULL;
    aout->flush = Flush;
    if (sio_onvol(sys->hdl, VolumeChanged, aout))
    {
        aout->volume_set = VolumeSet;
        aout->mute_set = MuteSet;
    }
    else
    {
        aout->volume_set = NULL;
        aout->mute_set = NULL;
    }
    sys->started = 0;
    sys->delay = 0;
    sio_onmove (sys->hdl, PositionChanged, aout);
    sio_start (sys->hdl);
    return VLC_SUCCESS;
error:
    sio_close (sys->hdl);
    return VLC_EGENERIC;
}
