}
static HRESULT Flush( aout_stream_sys_t *sys, bool drain)
{
    HRESULT ret = IDirectSoundBuffer_Stop( sys->p_dsbuffer );
    if( ret == DS_OK && !drain )
    {
        vlc_mutex_lock(&sys->lock);
        sys->i_data = 0;
        sys->i_last_read = sys->i_write;
        IDirectSoundBuffer_SetCurrentPosition( sys->p_dsbuffer, sys->i_write);
        sys->b_playing = false;
        vlc_mutex_unlock(&sys->lock);
    }
    return ret;
}
