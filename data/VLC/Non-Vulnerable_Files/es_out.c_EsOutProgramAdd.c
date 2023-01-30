 */
static es_out_pgrm_t *EsOutProgramAdd( es_out_t *out, int i_group )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    es_out_pgrm_t *p_pgrm = malloc( sizeof( es_out_pgrm_t ) );
    if( !p_pgrm )
        return NULL;
    /* Init */
    p_pgrm->i_id = i_group;
    p_pgrm->i_es = 0;
    p_pgrm->b_selected = false;
    p_pgrm->b_scrambled = false;
    p_pgrm->psz_name = NULL;
    p_pgrm->psz_now_playing = NULL;
    p_pgrm->psz_publisher = NULL;
    p_pgrm->p_clock = input_clock_New( p_sys->i_rate );
    if( !p_pgrm->p_clock )
    {
        free( p_pgrm );
        return NULL;
    }
    if( p_sys->b_paused )
        input_clock_ChangePause( p_pgrm->p_clock, p_sys->b_paused, p_sys->i_pause_date );
    input_clock_SetJitter( p_pgrm->p_clock, p_sys->i_pts_delay, p_sys->i_cr_average );
    /* Append it */
    TAB_APPEND( p_sys->i_pgrm, p_sys->pgrm, p_pgrm );
    /* Update "program" variable */
    if( EsOutIsProgramVisible( out, i_group ) )
        input_SendEventProgramAdd( p_input, i_group, NULL );
    if( i_group == p_sys->i_group_id || ( !p_sys->p_pgrm && p_sys->i_group_id == 0 ) )
        EsOutProgramSelect( out, p_pgrm );
    return p_pgrm;
}
