 */
static int EsOutProgramDel( es_out_t *out, int i_group )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    es_out_pgrm_t     *p_pgrm = NULL;
    int               i;
    for( i = 0; i < p_sys->i_pgrm; i++ )
    {
        if( p_sys->pgrm[i]->i_id == i_group )
        {
            p_pgrm = p_sys->pgrm[i];
            break;
        }
    }
    if( p_pgrm == NULL )
        return VLC_EGENERIC;
    if( p_pgrm->i_es )
    {
        msg_Dbg( p_input, "can't delete program %d which still has %i ES",
                 i_group, p_pgrm->i_es );
        return VLC_EGENERIC;
    }
    TAB_REMOVE( p_sys->i_pgrm, p_sys->pgrm, p_pgrm );
    /* If program is selected we need to unselect it */
    if( p_sys->p_pgrm == p_pgrm )
        p_sys->p_pgrm = NULL;
    input_clock_Delete( p_pgrm->p_clock );
    free( p_pgrm->psz_name );
    free( p_pgrm->psz_now_playing );
    free( p_pgrm->psz_publisher );
    free( p_pgrm );
    /* Update "program" variable */
    input_SendEventProgramDel( p_input, i_group );
    return VLC_SUCCESS;
}
