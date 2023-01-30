 *****************************************************************************/
static int Start(audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    SLresult       result;
    aout_sys_t *sys = aout->sys;
    // configure audio source - this defines the number of samples you can enqueue.
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
        SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
        OPENSLES_BUFFERS
    };
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType       = SL_DATAFORMAT_PCM;
    format_pcm.numChannels      = 2;
    format_pcm.samplesPerSec    = ((SLuint32) fmt->i_rate * 1000) ;
    format_pcm.bitsPerSample    = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.containerSize    = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.channelMask      = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    format_pcm.endianness       = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {
        SL_DATALOCATOR_OUTPUTMIX,
        sys->outputMixObject
    };
    SLDataSink audioSnk = {&loc_outmix, NULL};
    //create audio player
    const SLInterfaceID ids2[] = { sys->SL_IID_ANDROIDSIMPLEBUFFERQUEUE, sys->SL_IID_VOLUME };
    static const SLboolean req2[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    if (aout_get_native_sample_rate() >= fmt->i_rate) {
        result = CreateAudioPlayer(sys->engineEngine, &sys->playerObject, &audioSrc,
                                    &audioSnk, sizeof(ids2) / sizeof(*ids2),
                                    ids2, req2);
    } else {
        // Don't try to play back a sample rate higher than the native one,
        // since OpenSL ES will try to use the fast path, which AudioFlinger
        // will reject (fast path can't do resampling), and will end up with
        // too small buffers for the resampling. See http://b.android.com/59453
        // for details. This bug is still present in 4.4. If it is fixed later
        // this workaround could be made conditional.
        result = SL_RESULT_UNKNOWN_ERROR;
    }
    if (unlikely(result != SL_RESULT_SUCCESS)) {
        /* Try again with a more sensible samplerate */
        fmt->i_rate = 44100;
        format_pcm.samplesPerSec = ((SLuint32) 44100 * 1000) ;
        result = CreateAudioPlayer(sys->engineEngine, &sys->playerObject, &audioSrc,
                &audioSnk, sizeof(ids2) / sizeof(*ids2),
                ids2, req2);
    }
    CHECK_OPENSL_ERROR("Failed to create audio player");
    result = Realize(sys->playerObject, SL_BOOLEAN_FALSE);
    CHECK_OPENSL_ERROR("Failed to realize player object.");
    result = GetInterface(sys->playerObject, sys->SL_IID_PLAY, &sys->playerPlay);
    CHECK_OPENSL_ERROR("Failed to get player interface.");
    result = GetInterface(sys->playerObject, sys->SL_IID_VOLUME, &sys->volumeItf);
    CHECK_OPENSL_ERROR("failed to get volume interface.");
    result = GetInterface(sys->playerObject, sys->SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                  &sys->playerBufferQueue);
    CHECK_OPENSL_ERROR("Failed to get buff queue interface");
    result = RegisterCallback(sys->playerBufferQueue, PlayedCallback,
                                   (void*)aout);
    CHECK_OPENSL_ERROR("Failed to register buff queue callback.");
    // set the player's state to playing
    result = SetPlayState(sys->playerPlay, SL_PLAYSTATE_PLAYING);
    CHECK_OPENSL_ERROR("Failed to switch to playing state");
    /* XXX: rounding shouldn't affect us at normal sampling rate */
    sys->rate = fmt->i_rate;
    sys->samples_per_buf = OPENSLES_BUFLEN * fmt->i_rate / 1000;
    sys->buf = malloc(OPENSLES_BUFFERS * sys->samples_per_buf * bytesPerSample());
    if (!sys->buf)
        goto error;
    sys->started = false;
    sys->next_buf = 0;
    sys->p_buffer_chain = NULL;
    sys->pp_buffer_last = &sys->p_buffer_chain;
    sys->samples = 0;
    // we want 16bit signed data native endian.
    fmt->i_format              = VLC_CODEC_S16N;
    fmt->i_physical_channels   = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
    SetPositionUpdatePeriod(sys->playerPlay, AOUT_MIN_PREPARE_TIME * 1000 / CLOCK_FREQ);
    aout_FormatPrepare(fmt);
    return VLC_SUCCESS;
error:
    if (sys->playerObject) {
        Destroy(sys->playerObject);
        sys->playerObject = NULL;
    }
    return VLC_EGENERIC;
}
