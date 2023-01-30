}
static void UpdatePtsDelay( input_thread_t *p_input )
{
    input_thread_private_t *p_sys = p_input->p;
    /* Get max pts delay from input source */
    mtime_t i_pts_delay = p_sys->input.i_pts_delay;
    for( int i = 0; i < p_sys->i_slave; i++ )
        i_pts_delay = __MAX( i_pts_delay, p_sys->slave[i]->i_pts_delay );
    if( i_pts_delay < 0 )
        i_pts_delay = 0;
    /* Take care of audio/spu delay */
    const mtime_t i_audio_delay = var_GetTime( p_input, "audio-delay" );
    const mtime_t i_spu_delay   = var_GetTime( p_input, "spu-delay" );
    const mtime_t i_extra_delay = __MIN( i_audio_delay, i_spu_delay );
    if( i_extra_delay < 0 )
        i_pts_delay -= i_extra_delay;
    /* Update cr_average depending on the caching */
    const int i_cr_average = var_GetInteger( p_input, "cr-average" ) * i_pts_delay / DEFAULT_PTS_DELAY;
    /* */
    es_out_SetDelay( p_input->p->p_es_out_display, AUDIO_ES, i_audio_delay );
    es_out_SetDelay( p_input->p->p_es_out_display, SPU_ES, i_spu_delay );
    es_out_SetJitter( p_input->p->p_es_out, i_pts_delay, 0, i_cr_average );
}
