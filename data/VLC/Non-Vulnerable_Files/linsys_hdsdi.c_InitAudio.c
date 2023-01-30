}
static int InitAudio( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    es_format_t fmt;
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        hdsdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_channel == -1 ) continue;
        msg_Dbg( p_demux, "starting audio %u/%u rate:%u delay:%d",
                 1 + p_audio->i_channel / 2, 1 + (p_audio->i_channel % 2),
                 p_sys->i_sample_rate, p_audio->i_delay );
        es_format_Init( &fmt, AUDIO_ES, VLC_FOURCC('a','r','a','w') );
        fmt.i_id = p_audio->i_id;
        fmt.audio.i_channels          = 2;
        fmt.audio.i_original_channels =
        fmt.audio.i_physical_channels = AOUT_CHANS_STEREO;
        fmt.audio.i_rate              = p_sys->i_sample_rate;
        fmt.audio.i_bitspersample     = 16;
        fmt.audio.i_blockalign = fmt.audio.i_channels *
            fmt.audio.i_bitspersample / 8;
        fmt.i_bitrate = fmt.audio.i_channels * fmt.audio.i_rate *
            fmt.audio.i_bitspersample;
        p_audio->p_es = es_out_Add( p_demux->out, &fmt );
    }
    p_sys->i_next_adate = START_DATE;
    p_sys->i_ablock_size = p_sys->i_sample_rate * 4 * p_sys->i_frame_rate_base / p_sys->i_frame_rate;
    p_sys->i_aincr = 1000000. * p_sys->i_ablock_size / p_sys->i_sample_rate / 4;
    return VLC_SUCCESS;
}
