}
static int OutputStart( audio_output_t *p_aout,
                        audio_sample_format_t *restrict fmt )
{
    msg_Dbg( p_aout, "Opening DirectSound Audio Output" );
    /* Initialise DirectSound */
    if( InitDirectSound( p_aout ) )
    {
        msg_Err( p_aout, "cannot initialize DirectSound" );
        return -1;
    }
    HRESULT hr = Start( VLC_OBJECT(p_aout), &p_aout->sys->s, fmt );
    if( FAILED(hr) )
        return -1;
    /* Force volume update */
    VolumeSet( p_aout, p_aout->sys->volume.volume );
    MuteSet( p_aout, p_aout->sys->volume.mute );
    /* then launch the notification thread */
    p_aout->time_get = OutputTimeGet;
    p_aout->play = OutputPlay;
    p_aout->pause = OutputPause;
    p_aout->flush = OutputFlush;
    return 0;
}
