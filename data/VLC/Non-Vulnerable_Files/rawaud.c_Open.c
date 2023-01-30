 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    p_demux->p_sys      = p_sys = malloc( sizeof( demux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    es_format_Init( &p_sys->fmt, AUDIO_ES, 0 );
    char *psz_fourcc = var_CreateGetString( p_demux, "rawaud-fourcc" );
    p_sys->fmt.i_codec = vlc_fourcc_GetCodecFromString( AUDIO_ES, psz_fourcc );
    free( psz_fourcc );
    if( !p_sys->fmt.i_codec )
    {
        msg_Err( p_demux, "rawaud-fourcc must be a 4 character string");
        es_format_Clean( &p_sys->fmt );
        free( p_sys );
        return VLC_EGENERIC;
    }
    // get the bits per sample ratio based on codec
    switch( p_sys->fmt.i_codec )
    {
        case VLC_CODEC_FL64:
            p_sys->fmt.audio.i_bitspersample = 64;
            break;
        case VLC_CODEC_FL32:
        case VLC_CODEC_S32L:
        case VLC_CODEC_S32B:
            p_sys->fmt.audio.i_bitspersample = 32;
            break;
        case VLC_CODEC_S24L:
        case VLC_CODEC_S24B:
            p_sys->fmt.audio.i_bitspersample = 24;
            break;
        case VLC_CODEC_S16L:
        case VLC_CODEC_S16B:
            p_sys->fmt.audio.i_bitspersample = 16;
            break;
        case VLC_CODEC_S8:
        case VLC_CODEC_U8:
            p_sys->fmt.audio.i_bitspersample = 8;
            break;
        default:
            msg_Err( p_demux, "unknown fourcc format %4.4s",
                     (char *)&p_sys->fmt.i_codec);
            es_format_Clean( &p_sys->fmt );
            free( p_sys );
            return VLC_EGENERIC;
    }
    p_sys->fmt.psz_language = var_CreateGetString( p_demux, "rawaud-lang" );
    p_sys->fmt.audio.i_channels = var_CreateGetInteger( p_demux, "rawaud-channels" );
    p_sys->fmt.audio.i_rate = var_CreateGetInteger( p_demux, "rawaud-samplerate" );
    if( p_sys->fmt.audio.i_rate <= 0 )
    {
      msg_Err( p_demux, "invalid sample rate");
      es_format_Clean( &p_sys->fmt );
      free( p_sys );
      return VLC_EGENERIC;
    }
    if( p_sys->fmt.audio.i_channels <= 0 )
    {
      msg_Err( p_demux, "invalid number of channels");
      es_format_Clean( &p_sys->fmt );
      free( p_sys );
      return VLC_EGENERIC;
    }
    p_sys->fmt.i_bitrate = p_sys->fmt.audio.i_rate *
                           p_sys->fmt.audio.i_channels *
                           p_sys->fmt.audio.i_bitspersample;
    msg_Dbg( p_demux,
     "format initialized: channels=%d , samplerate=%d Hz, fourcc=%4.4s, bits per sample = %d, bitrate = %d bit/s",
     p_sys->fmt.audio.i_channels,
     p_sys->fmt.audio.i_rate,
     (char*)&p_sys->fmt.i_codec,
     p_sys->fmt.audio.i_bitspersample,
     p_sys->fmt.i_bitrate);
    /* add the es */
    p_sys->p_es = es_out_Add( p_demux->out, &p_sys->fmt );
    msg_Dbg( p_demux, "elementary stream added");
    /* initialize timing */
    date_Init( &p_sys->pts, p_sys->fmt.audio.i_rate, 1 );
    date_Set( &p_sys->pts, 0 );
    /* calculate 50ms frame size/time */
    p_sys->i_frame_samples = __MAX( p_sys->fmt.audio.i_rate / 20, 1 );
    p_sys->i_seek_step = p_sys->fmt.audio.i_channels *
                          ( (p_sys->fmt.audio.i_bitspersample + 7) / 8 );
    p_sys->i_frame_size = p_sys->i_frame_samples * p_sys->i_seek_step;
    msg_Dbg( p_demux, "frame size is %d bytes ", p_sys->i_frame_size);
    p_demux->pf_demux   = Demux;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
}
