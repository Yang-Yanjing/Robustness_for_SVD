 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_peek;
    if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
        return VLC_EGENERIC;
    if( memcmp( p_peek, "NSVf", 4 ) && memcmp( p_peek, "NSVs", 4 ) )
    {
       /* In case we had force this demuxer we try to resynch */
        if( !p_demux->b_force || ReSynch( p_demux ) )
            return VLC_EGENERIC;
    }
    p_sys = malloc( sizeof( demux_sys_t ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    /* Fill p_demux field */
    p_demux->p_sys = p_sys;
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    es_format_Init( &p_sys->fmt_audio, AUDIO_ES, 0 );
    p_sys->p_audio = NULL;
    es_format_Init( &p_sys->fmt_video, VIDEO_ES, 0 );
    p_sys->p_video = NULL;
    es_format_Init( &p_sys->fmt_sub, SPU_ES, 0 );
    p_sys->p_sub = NULL;
    p_sys->i_pcr   = 0;
    p_sys->i_time  = 0;
    p_sys->i_pcr_inc = 0;
    p_sys->b_start_record = false;
    return VLC_SUCCESS;
}
