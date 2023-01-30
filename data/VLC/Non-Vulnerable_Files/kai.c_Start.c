 *****************************************************************************/
static int Start ( audio_output_t *p_aout, audio_sample_format_t *fmt )
{
    aout_sys_t *p_sys = p_aout->sys;
    char *psz_mode;
    ULONG i_kai_mode;
    KAISPEC ks_wanted, ks_obtained;
    int i_nb_channels;
    int i_bytes_per_frame;
    vlc_value_t val, text;
    audio_sample_format_t format = *fmt;
    psz_mode = var_InheritString( p_aout, "kai-audio-device" );
    if( !psz_mode )
        psz_mode = ( char * )ppsz_kai_audio_device[ 0 ];  // "auto"
    i_kai_mode = KAIM_AUTO;
    if( strcmp( psz_mode, "dart" ) == 0 )
        i_kai_mode = KAIM_DART;
    else if( strcmp( psz_mode, "uniaud" ) == 0 )
        i_kai_mode = KAIM_UNIAUD;
    msg_Dbg( p_aout, "selected mode = %s", psz_mode );
    if( psz_mode != ppsz_kai_audio_device[ 0 ])
        free( psz_mode );
    i_nb_channels = aout_FormatNbChannels( &format );
    if ( i_nb_channels >= 2 )
    {
        /* KAI doesn't support more than two channels. */
        i_nb_channels = 2;
        format.i_physical_channels = AOUT_CHANS_STEREO;
    }
    else
        format.i_physical_channels = AOUT_CHAN_CENTER;
    /* Support S16 only */
    format.i_format = VLC_CODEC_S16N;
    aout_FormatPrepare( &format );
    i_bytes_per_frame = format.i_bytes_per_frame;
    /* Initialize library */
    if( kaiInit( i_kai_mode ))
    {
        msg_Err( p_aout, "cannot initialize KAI");
        return VLC_EGENERIC;
    }
    ks_wanted.usDeviceIndex   = 0;
    ks_wanted.ulType          = KAIT_PLAY;
    ks_wanted.ulBitsPerSample = BPS_16;
    ks_wanted.ulSamplingRate  = format.i_rate;
    ks_wanted.ulDataFormat    = MCI_WAVE_FORMAT_PCM;
    ks_wanted.ulChannels      = i_nb_channels;
    ks_wanted.ulNumBuffers    = 2;
    ks_wanted.ulBufferSize    = FRAME_SIZE * i_bytes_per_frame;
    ks_wanted.fShareable      = !var_InheritBool( p_aout,
                                                  "kai-audio-exclusive-mode");
    ks_wanted.pfnCallBack     = KaiCallback;
    ks_wanted.pCallBackData   = p_aout;
    msg_Dbg( p_aout, "requested ulBufferSize = %ld", ks_wanted.ulBufferSize );
    /* Open the sound device. */
    if( kaiOpen( &ks_wanted, &ks_obtained, &p_sys->hkai ))
    {
        msg_Err( p_aout, "cannot open KAI device");
        goto exit_kai_done;
    }
    msg_Dbg( p_aout, "open in %s mode",
             ks_obtained.fShareable ? "shareable" : "exclusive" );
    msg_Dbg( p_aout, "obtained i_nb_samples = %lu",
             ks_obtained.ulBufferSize / i_bytes_per_frame );
    msg_Dbg( p_aout, "obtained i_bytes_per_frame = %d",
             format.i_bytes_per_frame );
    p_sys->format = *fmt = format;
    p_aout->time_get = TimeGet;
    p_aout->play     = Play;
    p_aout->pause    = Pause;
    p_aout->flush    = Flush;
    aout_SoftVolumeStart( p_aout );
    CreateBuffer( p_aout, AUDIO_BUFFER_SIZE_IN_SECONDS *
                          format.i_rate * format.i_bytes_per_frame );
    /* Prevent SIG_FPE */
    _control87(MCW_EM, MCW_EM);
    return VLC_SUCCESS;
exit_kai_done :
    kaiDone();
    return VLC_EGENERIC;
}
