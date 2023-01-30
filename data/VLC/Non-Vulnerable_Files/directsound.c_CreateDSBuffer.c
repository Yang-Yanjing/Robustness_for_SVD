 */
static HRESULT CreateDSBuffer( vlc_object_t *obj, aout_stream_sys_t *sys,
                               int i_format, int i_channels, int i_nb_channels,
                               int i_rate, bool b_probe )
{
    WAVEFORMATEXTENSIBLE waveformat;
    DSBUFFERDESC         dsbdesc;
    HRESULT              hr;
    /* First set the sound buffer format */
    waveformat.dwChannelMask = 0;
    for( unsigned i = 0; pi_vlc_chan_order_wg4[i]; i++ )
        if( i_channels & pi_vlc_chan_order_wg4[i] )
            waveformat.dwChannelMask |= pi_channels_in[i];
    switch( i_format )
    {
    case VLC_CODEC_SPDIFL:
        i_nb_channels = 2;
        /* To prevent channel re-ordering */
        waveformat.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
        waveformat.Format.wBitsPerSample = 16;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_DOLBY_AC3_SPDIF;
        waveformat.SubFormat = _KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF;
        break;
    case VLC_CODEC_FL32:
        waveformat.Format.wBitsPerSample = sizeof(float) * 8;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        waveformat.SubFormat = _KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        break;
    case VLC_CODEC_S16N:
        waveformat.Format.wBitsPerSample = 16;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_PCM;
        waveformat.SubFormat = _KSDATAFORMAT_SUBTYPE_PCM;
        break;
    }
    waveformat.Format.nChannels = i_nb_channels;
    waveformat.Format.nSamplesPerSec = i_rate;
    waveformat.Format.nBlockAlign =
        waveformat.Format.wBitsPerSample / 8 * i_nb_channels;
    waveformat.Format.nAvgBytesPerSec =
        waveformat.Format.nSamplesPerSec * waveformat.Format.nBlockAlign;
    sys->i_bytes_per_sample = waveformat.Format.nBlockAlign;
    sys->format = i_format;
    /* Then fill in the direct sound descriptor */
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 /* Better position accuracy */
                    | DSBCAPS_GLOBALFOCUS         /* Allows background playing */
                    | DSBCAPS_CTRLVOLUME          /* Allows volume control */
                    | DSBCAPS_CTRLPOSITIONNOTIFY; /* Allow position notifications */
    /* Only use the new WAVE_FORMAT_EXTENSIBLE format for multichannel audio */
    if( i_nb_channels <= 2 )
    {
        waveformat.Format.cbSize = 0;
    }
    else
    {
        waveformat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        waveformat.Format.cbSize =
            sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        /* Needed for 5.1 on emu101k */
        dsbdesc.dwFlags |= DSBCAPS_LOCHARDWARE;
    }
    dsbdesc.dwBufferBytes = DS_BUF_SIZE; /* buffer size */
    dsbdesc.lpwfxFormat = (WAVEFORMATEX *)&waveformat;
    /* CreateSoundBuffer doesn't allow volume control for non-PCM buffers */
    if ( i_format == VLC_CODEC_SPDIFL )
        dsbdesc.dwFlags &= ~DSBCAPS_CTRLVOLUME;
    hr = IDirectSound_CreateSoundBuffer( sys->p_dsobject, &dsbdesc,
                                         &sys->p_dsbuffer, NULL );
    if( FAILED(hr) )
    {
        if( !(dsbdesc.dwFlags & DSBCAPS_LOCHARDWARE) )
            return hr;
        /* Try without DSBCAPS_LOCHARDWARE */
        dsbdesc.dwFlags &= ~DSBCAPS_LOCHARDWARE;
        hr = IDirectSound_CreateSoundBuffer( sys->p_dsobject, &dsbdesc,
                                             &sys->p_dsbuffer, NULL );
        if( FAILED(hr) )
            return hr;
        if( !b_probe )
            msg_Dbg( obj, "couldn't use hardware sound buffer" );
    }
    /* Stop here if we were just probing */
    if( b_probe )
    {
        IDirectSoundBuffer_Release( sys->p_dsbuffer );
        sys->p_dsbuffer = NULL;
        return DS_OK;
    }
    sys->i_rate = i_rate;
    sys->i_channel_mask = waveformat.dwChannelMask;
    sys->chans_to_reorder =
        aout_CheckChannelReorder( pi_channels_in, pi_channels_out,
                                  waveformat.dwChannelMask, sys->chan_table );
    if( sys->chans_to_reorder )
        msg_Dbg( obj, "channel reordering needed" );
    hr = IDirectSoundBuffer_QueryInterface( sys->p_dsbuffer,
                                            &IID_IDirectSoundNotify,
                                            (void **) &sys->p_notify );
    if( hr != DS_OK )
    {
        msg_Err( obj, "Couldn't query IDirectSoundNotify" );
        sys->p_notify = NULL;
    }
    FillBuffer( obj, sys, NULL );
    return DS_OK;
}
