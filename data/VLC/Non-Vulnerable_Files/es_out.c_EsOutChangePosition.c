}
static void EsOutChangePosition( es_out_t *out )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_SendEventCache( p_sys->p_input, 0.0 );
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        es_out_id_t *p_es = p_sys->es[i];
        if( !p_es->p_dec )
            continue;
        input_DecoderStartWait( p_es->p_dec );
        if( p_es->p_dec_record )
            input_DecoderStartWait( p_es->p_dec_record );
    }
    for( int i = 0; i < p_sys->i_pgrm; i++ )
        input_clock_Reset( p_sys->pgrm[i]->p_clock );
    p_sys->b_buffering = true;
    p_sys->i_buffering_extra_initial = 0;
    p_sys->i_buffering_extra_stream = 0;
    p_sys->i_buffering_extra_system = 0;
    p_sys->i_preroll_end = -1;
}
