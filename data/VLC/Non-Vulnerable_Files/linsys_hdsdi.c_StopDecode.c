};
static void StopDecode( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    es_out_Del( p_demux->out, p_sys->p_es_video );
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        hdsdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_channel != -1 && p_audio->p_es != NULL )
        {
            es_out_Del( p_demux->out, p_audio->p_es );
            p_audio->p_es = NULL;
        }
    }
}
