/* FIXME handle properly string (convert to utf8) */
static void PATCallBack( scan_session_t *p_session, dvbpsi_pat_t *p_pat )
{
    vlc_object_t *p_obj = p_session->p_obj;
    msg_Dbg( p_obj, "PATCallBack" );
    /* */
    if( p_session->p_pat && p_session->p_pat->b_current_next )
    {
        dvbpsi_DeletePAT( p_session->p_pat );
        p_session->p_pat = NULL;
    }
    if( p_session->p_pat )
    {
        dvbpsi_DeletePAT( p_pat );
        return;
    }
    dvbpsi_pat_program_t *p_program;
    /* */
    p_session->p_pat = p_pat;
    /* */
    msg_Dbg( p_obj, "new PAT ts_id=%d version=%d current_next=%d",
             p_pat->i_ts_id, p_pat->i_version, p_pat->b_current_next );
    for( p_program = p_pat->p_first_program; p_program != NULL; p_program = p_program->p_next )
    {
        msg_Dbg( p_obj, "  * number=%d pid=%d", p_program->i_number, p_program->i_pid );
        if( p_program->i_number == 0 )
            p_session->i_nit_pid = p_program->i_pid;
    }
}
