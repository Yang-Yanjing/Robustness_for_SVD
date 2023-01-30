}
static void StopDecode( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if ( p_sys->i_state != STATE_SYNC )
        return;
    free( p_sys->p_line_buffer );
    block_Release( p_sys->p_current_picture );
    p_sys->p_current_picture = NULL;
    es_out_Del( p_demux->out, p_sys->p_es_video );
    if ( p_sys->b_vbi )
    {
        free( p_sys->p_wss_buffer );
        p_sys->p_wss_buffer = NULL;
        vbi_raw_decoder_destroy( &p_sys->rd_wss );
        if ( p_sys->p_es_telx )
        {
            es_out_Del( p_demux->out, p_sys->p_es_telx );
            free( p_sys->p_telx_buffer );
            p_sys->p_telx_buffer = NULL;
            vbi_raw_decoder_destroy( &p_sys->rd_telx );
        }
    }
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        sdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_group && p_audio->p_es != NULL )
        {
            es_out_Del( p_demux->out, p_audio->p_es );
            p_audio->p_es = NULL;
            free( p_audio->p_buffer );
            p_audio->p_buffer = NULL;
        }
    }
}
