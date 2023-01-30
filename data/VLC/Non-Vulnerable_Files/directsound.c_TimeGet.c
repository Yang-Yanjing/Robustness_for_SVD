static HRESULT Flush( aout_stream_sys_t *sys, bool drain);
static HRESULT TimeGet( aout_stream_sys_t *sys, mtime_t *delay )
{
    DWORD read, status;
    HRESULT hr;
    mtime_t size;
    hr = IDirectSoundBuffer_GetStatus( sys->p_dsbuffer, &status );
    if( hr != DS_OK )
        return hr;
    if( !(status & DSBSTATUS_PLAYING) )
        return DSERR_INVALIDCALL ;
    hr = IDirectSoundBuffer_GetCurrentPosition( sys->p_dsbuffer, &read, NULL );
    if( hr != DS_OK )
        return hr;
    size = (mtime_t)read - sys->i_last_read;
    /* GetCurrentPosition cannot be trusted if the return doesn't change
     * Just return an error */
    if( size ==  0 )
        return DSERR_GENERIC ;
    else if( size < 0 )
      size += DS_BUF_SIZE;
    sys->i_data -= size;
    sys->i_last_read = read;
    if( sys->i_data < 0 )
        /* underrun */
        Flush(sys, false);
    *delay = ( sys->i_data / sys->i_bytes_per_sample ) * CLOCK_FREQ / sys->i_rate;
    return DS_OK;
}
