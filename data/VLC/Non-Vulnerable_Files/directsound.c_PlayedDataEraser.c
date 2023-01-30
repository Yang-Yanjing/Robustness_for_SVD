}
static void * PlayedDataEraser( void * data )
{
    const audio_output_t *aout = (audio_output_t *) data;
    aout_stream_sys_t *p_sys = &aout->sys->s;
    void *p_write_position, *p_wrap_around;
    unsigned long l_bytes1, l_bytes2;
    DWORD i_read;
    int64_t toerase, tosleep;
    HRESULT dsresult;
    for(;;)
    {
        int canc = vlc_savecancel();
        vlc_mutex_lock( &p_sys->lock );
        while( !p_sys->b_playing )
           vlc_cond_wait( &p_sys->cond, &p_sys->lock );
        toerase = 0;
        tosleep = 0;
        dsresult = IDirectSoundBuffer_GetCurrentPosition( p_sys->p_dsbuffer,
                                                          &i_read, NULL );
        if( dsresult == DS_OK )
        {
            int64_t max = (int64_t) i_read - (int64_t) p_sys->i_write;
            tosleep = -max;
            if( max <= 0 )
                max += DS_BUF_SIZE;
            else
                tosleep += DS_BUF_SIZE;
            toerase = max;
            tosleep = ( tosleep / p_sys->i_bytes_per_sample ) * CLOCK_FREQ / p_sys->i_rate;
        }
        tosleep = __MAX( tosleep, 20000 );
        dsresult = IDirectSoundBuffer_Lock( p_sys->p_dsbuffer,
                                            p_sys->i_write,
                                            toerase,
                                            &p_write_position,
                                            &l_bytes1,
                                            &p_wrap_around,
                                            &l_bytes2,
                                            0 );
        if( dsresult == DSERR_BUFFERLOST )
        {
            IDirectSoundBuffer_Restore( p_sys->p_dsbuffer );
            dsresult = IDirectSoundBuffer_Lock( p_sys->p_dsbuffer,
                                                p_sys->i_write,
                                                toerase,
                                                &p_write_position,
                                                &l_bytes1,
                                                &p_wrap_around,
                                                &l_bytes2,
                                                0 );
        }
        if( dsresult != DS_OK )
            goto wait;
        memset( p_write_position, 0, l_bytes1 );
        memset( p_wrap_around, 0, l_bytes2 );
        IDirectSoundBuffer_Unlock( p_sys->p_dsbuffer, p_write_position, l_bytes1,
                                   p_wrap_around, l_bytes2 );
wait:
        vlc_mutex_unlock(&p_sys->lock);
        vlc_restorecancel(canc);
        msleep(tosleep);
    }
    return NULL;
}
