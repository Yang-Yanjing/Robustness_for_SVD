}
static HRESULT StreamStart( aout_stream_t *s,
                            audio_sample_format_t *restrict fmt,
                            const GUID *sid )
{
    aout_stream_sys_t *sys = calloc( 1, sizeof( *sys ) );
    if( unlikely(sys == NULL) )
        return E_OUTOFMEMORY;
    DIRECTX_AUDIO_ACTIVATION_PARAMS params = {
        .cbDirectXAudioActivationParams = sizeof( params ),
        .guidAudioSession = *sid,
        .dwAudioStreamFlags = 0,
    };
    PROPVARIANT prop;
    PropVariantInit( &prop );
    prop.vt = VT_BLOB;
    prop.blob.cbSize = sizeof( params );
    prop.blob.pBlobData = (BYTE *)&params;
    void *pv;
    HRESULT hr = aout_stream_Activate( s, &IID_IDirectSound, &prop, &pv );
    if( FAILED(hr) )
        goto error;
    sys->p_dsobject = pv;
    hr = Start( VLC_OBJECT(s), sys, fmt );
    if( FAILED(hr) )
        goto error;
    s->sys = sys;
    s->time_get = StreamTimeGet;
    s->play = StreamPlay;
    s->pause = StreamPause;
    s->flush = StreamFlush;
    return S_OK;
error:
    free( sys );
    return hr;
}
