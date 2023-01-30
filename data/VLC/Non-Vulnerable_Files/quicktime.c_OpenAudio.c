 *****************************************************************************/
static int OpenAudio( decoder_t *p_dec )
{
    decoder_sys_t *p_sys;
    int             i_error;
    char            fcc[4];
    unsigned long   WantedBufferSize;
    unsigned long   InputBufferSize = 0;
    unsigned long   OutputBufferSize = 0;
    /* get lock, avoid segfault */
    vlc_mutex_lock( &qt_mutex );
    p_sys = calloc( 1, sizeof( decoder_sys_t ) );
    p_dec->p_sys = p_sys;
    p_dec->pf_decode_audio = DecodeAudio;
    if( p_dec->fmt_in.i_original_fourcc )
        memcpy( fcc, &p_dec->fmt_in.i_original_fourcc, 4 );
    else
        memcpy( fcc, &p_dec->fmt_in.i_codec, 4 );
#ifdef __APPLE__
    EnterMovies();
#endif
    if( QTAudioInit( p_dec ) )
    {
        msg_Err( p_dec, "cannot initialize QT");
        goto exit_error;
    }
#ifndef __APPLE__
    if( ( i_error = p_sys->InitializeQTML( 6 + 16 ) ) )
    {
        msg_Dbg( p_dec, "error on InitializeQTML = %d", i_error );
        goto exit_error;
    }
#endif
    /* input format settings */
    p_sys->InputFormatInfo.flags       = 0;
    p_sys->InputFormatInfo.sampleCount = 0;
    p_sys->InputFormatInfo.buffer      = NULL;
    p_sys->InputFormatInfo.reserved    = 0;
    p_sys->InputFormatInfo.numChannels = p_dec->fmt_in.audio.i_channels;
    p_sys->InputFormatInfo.sampleSize  = p_dec->fmt_in.audio.i_bitspersample;
    p_sys->InputFormatInfo.sampleRate  = p_dec->fmt_in.audio.i_rate;
    p_sys->InputFormatInfo.format      = FCC( fcc[0], fcc[1], fcc[2], fcc[3] );
    /* output format settings */
    p_sys->OutputFormatInfo.flags       = 0;
    p_sys->OutputFormatInfo.sampleCount = 0;
    p_sys->OutputFormatInfo.buffer      = NULL;
    p_sys->OutputFormatInfo.reserved    = 0;
    p_sys->OutputFormatInfo.numChannels = p_dec->fmt_in.audio.i_channels;
    p_sys->OutputFormatInfo.sampleSize  = 16;
    p_sys->OutputFormatInfo.sampleRate  = p_dec->fmt_in.audio.i_rate;
    p_sys->OutputFormatInfo.format      = FCC( 'N', 'O', 'N', 'E' );
    i_error = p_sys->SoundConverterOpen( &p_sys->InputFormatInfo,
                                         &p_sys->OutputFormatInfo,
                                         &p_sys->myConverter );
    if( i_error )
    {
        msg_Err( p_dec, "error on SoundConverterOpen = %d", i_error );
        goto exit_error;
    }
#if 0
    /* tell the sound converter we accept VBR formats */
    i_error = SoundConverterSetInfo( p_dec->myConverter, siClientAcceptsVBR,
                                     (void *)true );
#endif
    if( p_dec->fmt_in.i_extra > 36 + 8 )
    {
        i_error = p_sys->SoundConverterSetInfo( p_sys->myConverter,
                                                FCC( 'w', 'a', 'v', 'e' ),
                                                ((uint8_t*)p_dec->fmt_in.p_extra) + 36 + 8 );
        msg_Dbg( p_dec, "error on SoundConverterSetInfo = %d", i_error );
    }
    WantedBufferSize = p_sys->OutputFormatInfo.numChannels *
                       p_sys->OutputFormatInfo.sampleRate * 2;
    p_sys->FramesToGet = 0;
    i_error = p_sys->SoundConverterGetBufferSizes( p_sys->myConverter,
                                                   WantedBufferSize,
                                                   &p_sys->FramesToGet,
                                                   &InputBufferSize,
                                                   &OutputBufferSize );
    msg_Dbg( p_dec, "WantedBufferSize=%li InputBufferSize=%li "
             "OutputBufferSize=%li FramesToGet=%li",
             WantedBufferSize, InputBufferSize, OutputBufferSize,
             p_sys->FramesToGet );
    p_sys->InFrameSize  = (InputBufferSize + p_sys->FramesToGet - 1 ) /
                            p_sys->FramesToGet;
    p_sys->OutFrameSize = OutputBufferSize / p_sys->FramesToGet;
    msg_Dbg( p_dec, "frame size %d -> %d",
             p_sys->InFrameSize, p_sys->OutFrameSize );
    if( (i_error = p_sys->SoundConverterBeginConversion(p_sys->myConverter)) )
    {
        msg_Err( p_dec,
                 "error on SoundConverterBeginConversion = %d", i_error );
        goto exit_error;
    }
    es_format_Init( &p_dec->fmt_out, AUDIO_ES, VLC_CODEC_S16N );
    p_dec->fmt_out.audio.i_rate = p_sys->OutputFormatInfo.sampleRate;
    p_dec->fmt_out.audio.i_channels = p_sys->OutputFormatInfo.numChannels;
    p_dec->fmt_out.audio.i_physical_channels =
    p_dec->fmt_out.audio.i_original_channels =
        pi_channels_maps[p_sys->OutputFormatInfo.numChannels];
    date_Init( &p_sys->date, p_dec->fmt_out.audio.i_rate, 1 );
    p_sys->i_buffer      = 0;
    p_sys->i_buffer_size = 100*1000;
    p_sys->p_buffer      = malloc( p_sys->i_buffer_size );
    if( !p_sys->p_buffer )
        goto exit_error;
    p_sys->i_out = 0;
    p_sys->i_out_frames = 0;
    vlc_mutex_unlock( &qt_mutex );
    return VLC_SUCCESS;
exit_error:
#ifdef LOADER
    Restore_LDT_Keeper( p_sys->ldt_fs );
#endif
    vlc_mutex_unlock( &qt_mutex );
    free( p_sys );
    return VLC_EGENERIC;
}
