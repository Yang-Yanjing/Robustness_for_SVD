 */
static void EsOutProgramSelect( es_out_t *out, es_out_pgrm_t *p_pgrm )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    int               i;
    if( p_sys->p_pgrm == p_pgrm )
        return; /* Nothing to do */
    if( p_sys->p_pgrm )
    {
        es_out_pgrm_t *old = p_sys->p_pgrm;
        msg_Dbg( p_input, "unselecting program id=%d", old->i_id );
        for( i = 0; i < p_sys->i_es; i++ )
        {
            if( p_sys->es[i]->p_pgrm == old && EsIsSelected( p_sys->es[i] ) &&
                p_sys->i_mode != ES_OUT_MODE_ALL )
                EsUnselect( out, p_sys->es[i], true );
        }
        p_sys->p_es_audio = NULL;
        p_sys->p_es_sub = NULL;
        p_sys->p_es_video = NULL;
    }
    msg_Dbg( p_input, "selecting program id=%d", p_pgrm->i_id );
    /* Mark it selected */
    p_pgrm->b_selected = true;
    /* Switch master stream */
    p_sys->p_pgrm = p_pgrm;
    /* Update "program" */
    input_SendEventProgramSelect( p_input, p_pgrm->i_id );
    /* Update "es-*" */
    input_SendEventEsDel( p_input, AUDIO_ES, -1 );
    input_SendEventEsDel( p_input, VIDEO_ES, -1 );
    input_SendEventEsDel( p_input, SPU_ES, -1 );
    input_SendEventTeletextDel( p_input, -1 );
    input_SendEventProgramScrambled( p_input, p_pgrm->i_id, p_pgrm->b_scrambled );
    /* TODO event */
    var_SetInteger( p_input, "teletext-es", -1 );
    for( i = 0; i < p_sys->i_es; i++ )
    {
        if( p_sys->es[i]->p_pgrm == p_sys->p_pgrm )
            EsOutESVarUpdate( out, p_sys->es[i], false );
        EsOutSelect( out, p_sys->es[i], false );
    }
    /* Update now playing */
    input_item_SetESNowPlaying( p_input->p->p_item, p_pgrm->psz_now_playing );
    input_item_SetPublisher( p_input->p->p_item, p_pgrm->psz_publisher );
    input_SendEventMeta( p_input );
}
