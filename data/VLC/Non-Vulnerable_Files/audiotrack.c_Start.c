}
static int Start(audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    struct aout_sys_t *p_sys = aout->sys;
    int status, size;
    int afSampleRate, afFrameCount, afLatency, minBufCount, minFrameCount;
    int stream_type, channel, rate, format;
    /* 4000 <= frequency <= 48000 */
    rate = fmt->i_rate;
    if (rate < 4000)
        rate = 4000;
    if (rate > 48000)
        rate = 48000;
    stream_type = MUSIC;
    /* We can only accept U8 and S16N */
    if (fmt->i_format != VLC_CODEC_U8 && fmt->i_format != VLC_CODEC_S16N)
        fmt->i_format = VLC_CODEC_S16N;
    format = (fmt->i_format == VLC_CODEC_S16N) ? PCM_16_BIT : PCM_8_BIT;
    /* TODO: android supports more channels */
    fmt->i_original_channels = fmt->i_physical_channels;
    switch(aout_FormatNbChannels(fmt))
    {
    case 1:
        channel = CHANNEL_OUT_MONO;
        fmt->i_physical_channels = AOUT_CHAN_CENTER;
        break;
    case 2:
    default:
        channel = CHANNEL_OUT_STEREO;
        fmt->i_physical_channels = AOUT_CHANS_STEREO;
        break;
    }
    /* Get the minimum buffer value */
    if (!p_sys->at_getMinFrameCount) {
        status = p_sys->as_getOutputSamplingRate(&afSampleRate, stream_type);
        status ^= p_sys->as_getOutputFrameCount(&afFrameCount, stream_type);
        status ^= p_sys->as_getOutputLatency((uint32_t*)(&afLatency), stream_type);
        if (status != 0) {
            msg_Err(aout, "Could not query the AudioStream parameters");
            return VLC_EGENERIC;
        }
        minBufCount = afLatency / ((1000 * afFrameCount) / afSampleRate);
        if (minBufCount < 2)
            minBufCount = 2;
        minFrameCount = (afFrameCount * rate * minBufCount) / afSampleRate;
    }
    else {
        status = p_sys->at_getMinFrameCount(&minFrameCount, stream_type, rate);
        if (status != 0) {
            msg_Err(aout, "Could not query the AudioTrack parameters");
            return VLC_EGENERIC;
        }
    }
    size = minFrameCount * (channel == CHANNEL_OUT_STEREO ? 2 : 1) * 4;
    /* Sizeof(AudioTrack) == 0x58 (not sure) on 2.2.1, this should be enough */
    p_sys->AudioTrack = malloc(SIZE_OF_AUDIOTRACK);
    if (!p_sys->AudioTrack)
        return VLC_ENOMEM;
    *((uint32_t *) ((uint32_t)p_sys->AudioTrack + SIZE_OF_AUDIOTRACK - 4)) = 0xbaadbaad;
    // Higher than android 2.2
    if (p_sys->at_ctor)
        p_sys->at_ctor(p_sys->AudioTrack, stream_type, rate, format, channel, size, 0, NULL, NULL, 0, 0);
    // Higher than android 1.6
    else if (p_sys->at_ctor_legacy)
        p_sys->at_ctor_legacy(p_sys->AudioTrack, stream_type, rate, format, channel, size, 0, NULL, NULL, 0);
    assert( (*((uint32_t *) ((uint32_t)p_sys->AudioTrack + SIZE_OF_AUDIOTRACK - 4)) == 0xbaadbaad) );
    /* And Init */
    status = p_sys->at_initCheck(p_sys->AudioTrack);
    /* android 1.6 uses channel count instead of stream_type */
    if (status != 0) {
        channel = (channel == CHANNEL_OUT_STEREO) ? 2 : 1;
        p_sys->at_ctor_legacy(p_sys->AudioTrack, stream_type, rate, format, channel, size, 0, NULL, NULL, 0);
        status = p_sys->at_initCheck(p_sys->AudioTrack);
    }
    if (status != 0) {
        msg_Err(aout, "Cannot create AudioTrack!");
        free(p_sys->AudioTrack);
        return VLC_EGENERIC;
    }
    aout_SoftVolumeStart(aout);
    aout->sys = p_sys;
    aout->play = Play;
    aout->pause = Pause;
    aout->flush = Flush;
    aout->time_get = TimeGet;
    p_sys->rate = rate;
    p_sys->samples_written = 0;
    p_sys->bytes_per_frame = aout_FormatNbChannels(fmt) * (format == PCM_16_BIT) ? 2 : 1;
    p_sys->at_start(p_sys->AudioTrack);
    TimeGet(aout, NULL); /* Gets the initial value of DAC samples counter */
    fmt->i_rate = rate;
    return VLC_SUCCESS;
}
