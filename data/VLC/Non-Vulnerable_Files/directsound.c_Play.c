}
static HRESULT Play( vlc_object_t *obj, aout_stream_sys_t *sys,
                     block_t *p_buffer )
{
    HRESULT dsresult;
    dsresult = FillBuffer( obj, sys, p_buffer );
    if( dsresult != DS_OK )
        return dsresult;
    /* start playing the buffer */
    dsresult = IDirectSoundBuffer_Play( sys->p_dsbuffer, 0, 0,
                                        DSBPLAY_LOOPING );
    if( dsresult == DSERR_BUFFERLOST )
    {
        IDirectSoundBuffer_Restore( sys->p_dsbuffer );
        dsresult = IDirectSoundBuffer_Play( sys->p_dsbuffer,
                                            0, 0, DSBPLAY_LOOPING );
    }
    if( dsresult != DS_OK )
        msg_Err( obj, "cannot start playing buffer: (hr=0x%0lx)", dsresult );
    else
    {
        vlc_mutex_lock( &sys->lock );
        sys->b_playing = true;
        vlc_cond_signal(&sys->cond);
        vlc_mutex_unlock( &sys->lock );
    }
    return dsresult;
}
