}
static int InitAudio( demux_t *p_demux, sdi_audio_t *p_audio )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    es_format_t fmt;
    msg_Dbg( p_demux, "starting audio %u/%u rate:%u delay:%d", p_audio->i_group,
             p_audio->i_pair, p_audio->i_rate, p_audio->i_delay );
    es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_S16L );
    fmt.i_id = p_audio->i_id;
    fmt.audio.i_channels          = 2;
    fmt.audio.i_original_channels =
    fmt.audio.i_physical_channels = AOUT_CHANS_STEREO;
    fmt.audio.i_rate              = p_audio->i_rate;
    fmt.audio.i_bitspersample     = 16;
    fmt.audio.i_blockalign        = fmt.audio.i_channels *
                                    fmt.audio.i_bitspersample / 8;
    fmt.i_bitrate                 = fmt.audio.i_channels * fmt.audio.i_rate *
                                    fmt.audio.i_bitspersample;
    p_audio->p_es                 = es_out_Add( p_demux->out, &fmt );
    p_audio->i_nb_samples         = p_audio->i_rate * p_sys->i_frame_rate_base
                                    / p_sys->i_frame_rate;
    p_audio->i_max_samples        = (float)p_audio->i_nb_samples *
                                    (1. + SAMPLERATE_TOLERANCE);
    p_audio->p_buffer             = malloc( p_audio->i_max_samples * sizeof(int16_t) * 2 );
    p_audio->i_left_samples       = p_audio->i_right_samples = 0;
    p_audio->i_block_number       = 0;
    if( unlikely( !p_audio->p_buffer ) )
        return VLC_ENOMEM;
    return VLC_SUCCESS;
}
