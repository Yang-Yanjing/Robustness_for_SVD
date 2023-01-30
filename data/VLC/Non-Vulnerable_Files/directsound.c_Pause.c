}
static HRESULT Pause( aout_stream_sys_t *sys, bool pause )
{
    HRESULT hr;
    if( pause )
        hr = IDirectSoundBuffer_Stop( sys->p_dsbuffer );
    else
        hr = IDirectSoundBuffer_Play( sys->p_dsbuffer, 0, 0, DSBPLAY_LOOPING );
    if( hr == DS_OK )
    {
        vlc_mutex_lock( &sys->lock );
        sys->b_playing = !pause;
        if( sys->b_playing )
            vlc_cond_signal( &sys->cond );
        vlc_mutex_unlock( &sys->lock );
    }
    return hr;
}
