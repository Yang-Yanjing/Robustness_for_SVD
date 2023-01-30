}
static void EsOutTerminate( es_out_t *out )
{
    es_out_sys_t *p_sys = out->p_sys;
    if( p_sys->p_sout_record )
        EsOutSetRecord( out, false );
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        if( p_sys->es[i]->p_dec )
            input_DecoderDelete( p_sys->es[i]->p_dec );
        free( p_sys->es[i]->psz_language );
        free( p_sys->es[i]->psz_language_code );
        es_format_Clean( &p_sys->es[i]->fmt );
        free( p_sys->es[i] );
    }
    TAB_CLEAN( p_sys->i_es, p_sys->es );
    /* FIXME duplicate work EsOutProgramDel (but we cannot use it) add a EsOutProgramClean ? */
    for( int i = 0; i < p_sys->i_pgrm; i++ )
    {
        es_out_pgrm_t *p_pgrm = p_sys->pgrm[i];
        input_clock_Delete( p_pgrm->p_clock );
        free( p_pgrm->psz_now_playing );
        free( p_pgrm->psz_publisher );
        free( p_pgrm->psz_name );
        free( p_pgrm );
    }
    TAB_CLEAN( p_sys->i_pgrm, p_sys->pgrm );
    p_sys->p_pgrm = NULL;
    input_item_SetEpgOffline( p_sys->p_input->p->p_item );
    input_SendEventMetaEpg( p_sys->p_input );
}
