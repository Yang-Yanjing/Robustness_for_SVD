 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    es_format_t  fmt;
    const uint8_t *p_peek;
    if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 ) return VLC_EGENERIC;
    if( p_peek[0] != 'A' || p_peek[1] != 'V' || p_peek[4] != 0x55 )
    {
        /* In case we had forced this demuxer we try to resynch */
        if( !p_demux->b_force || ReSynch( p_demux ) )
            return VLC_EGENERIC;
    }
    p_sys = malloc( sizeof( demux_sys_t ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    /* Fill p_demux field */
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys = p_sys;
    /* Register one audio and one video stream */
    es_format_Init( &fmt, AUDIO_ES, VLC_CODEC_MPGA );
    fmt.b_packetized = false;
    p_sys->p_audio = es_out_Add( p_demux->out, &fmt );
    es_format_Init( &fmt, VIDEO_ES, VLC_CODEC_MPGV );
    fmt.b_packetized = false;
    p_sys->p_video = es_out_Add( p_demux->out, &fmt );
    p_sys->i_vc    = -1;
    p_sys->i_ac    = -1;
    p_sys->p_pes   = NULL;
    p_sys->p_es    = NULL;
    p_sys->b_pcr_audio = false;
    return VLC_SUCCESS;
}
