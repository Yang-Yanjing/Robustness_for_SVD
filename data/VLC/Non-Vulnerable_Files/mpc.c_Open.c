 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    es_format_t fmt;
    const uint8_t *p_peek;
    if( stream_Peek( p_demux->s, &p_peek, 4 ) < 4 )
        return VLC_EGENERIC;
    if( memcmp( p_peek, "MP+", 3 )
#ifdef HAVE_MPC_MPCDEC_H
        /* SV8 format */
        && memcmp( p_peek, "MPCK", 4 )
#endif
      )
    {
        /* for v4..6 we check extension file */
        const int i_version = (GetDWLE( p_peek ) >> 11)&0x3ff;
        if( i_version  < 4 || i_version > 6 )
            return VLC_EGENERIC;
        if( !p_demux->b_force )
        {
            /* Check file name extension */
            if( !demux_IsPathExtension( p_demux, ".mpc" ) &&
                !demux_IsPathExtension( p_demux, ".mp+" ) &&
                !demux_IsPathExtension( p_demux, ".mpp" ) )
                return VLC_EGENERIC;
        }
    }
    /* */
    p_sys = calloc( 1, sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_position = 0;
    p_sys->reader.read = ReaderRead;
    p_sys->reader.seek = ReaderSeek;
    p_sys->reader.tell = ReaderTell;
    p_sys->reader.get_size = ReaderGetSize;
    p_sys->reader.canseek = ReaderCanSeek;
    p_sys->reader.data = p_demux;
#ifndef HAVE_MPC_MPCDEC_H
    /* Load info */
    mpc_streaminfo_init( &p_sys->info );
    if( mpc_streaminfo_read( &p_sys->info, &p_sys->reader ) != ERROR_CODE_OK )
        goto error;
    /* */
    mpc_decoder_setup( &p_sys->decoder, &p_sys->reader );
    if( !mpc_decoder_initialize( &p_sys->decoder, &p_sys->info ) )
        goto error;
#else
    p_sys->decoder = mpc_demux_init( &p_sys->reader );
    if( !p_sys->decoder )
        goto error;
    mpc_demux_get_info( p_sys->decoder, &p_sys->info );
#endif
    /* Fill p_demux fields */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys;
    /* */
#ifndef MPC_FIXED_POINT
    es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_FL32 );
#else
#   ifdef WORDS_BIGENDIAN
    es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_S32B );
#   else
    es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_S32L );
#   endif
#endif
    fmt.audio.i_channels = p_sys->info.channels;
    fmt.audio.i_rate = p_sys->info.sample_freq;
    fmt.audio.i_blockalign = 4*fmt.audio.i_channels;
    fmt.audio.i_bitspersample = 32;
    fmt.i_bitrate = fmt.i_bitrate * fmt.audio.i_channels *
                    fmt.audio.i_bitspersample;
#ifdef HAVE_MPC_MPCDEC_H
#   define CONVERT_PEAK( mpc_peak ) (pow( 10, (mpc_peak) / 256.0 / 20.0 ) / 32767.0)
#   define CONVERT_GAIN( mpc_gain ) (MPC_OLD_GAIN_REF - (mpc_gain) / 256.0)
#else
#   define CONVERT_PEAK( mpc_peak ) ((mpc_peak) / 32767.0)
#   define CONVERT_GAIN( mpc_gain ) ((mpc_gain) / 100.0)
#endif
    if( p_sys->info.peak_title > 0 )
    {
        fmt.audio_replay_gain.pb_peak[AUDIO_REPLAY_GAIN_TRACK] = true;
        fmt.audio_replay_gain.pf_peak[AUDIO_REPLAY_GAIN_TRACK] = (float) CONVERT_PEAK( p_sys->info.peak_title );
        fmt.audio_replay_gain.pb_gain[AUDIO_REPLAY_GAIN_TRACK] = true;
        fmt.audio_replay_gain.pf_gain[AUDIO_REPLAY_GAIN_TRACK] = (float) CONVERT_GAIN( p_sys->info.gain_title );
    }
    if( p_sys->info.peak_album > 0 )
    {
        fmt.audio_replay_gain.pb_peak[AUDIO_REPLAY_GAIN_ALBUM] = true;
        fmt.audio_replay_gain.pf_peak[AUDIO_REPLAY_GAIN_ALBUM] = (float) CONVERT_PEAK( p_sys->info.peak_album );
        fmt.audio_replay_gain.pb_gain[AUDIO_REPLAY_GAIN_ALBUM] = true;
        fmt.audio_replay_gain.pf_gain[AUDIO_REPLAY_GAIN_ALBUM] = (float) CONVERT_GAIN( p_sys->info.gain_album );
    }
#undef CONVERT_GAIN
#undef CONVERT_PEAK
    p_sys->p_es = es_out_Add( p_demux->out, &fmt );
    if( !p_sys->p_es )
        goto error;
    return VLC_SUCCESS;
error:
    free( p_sys );
    return VLC_EGENERIC;
}
