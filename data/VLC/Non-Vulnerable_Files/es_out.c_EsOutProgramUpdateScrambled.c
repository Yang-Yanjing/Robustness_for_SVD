}
static void EsOutProgramUpdateScrambled( es_out_t *p_out, es_out_pgrm_t *p_pgrm )
{
    es_out_sys_t    *p_sys = p_out->p_sys;
    input_thread_t  *p_input = p_sys->p_input;
    bool b_scrambled = false;
    for( int i = 0; i < p_sys->i_es; i++ )
    {
        if( p_sys->es[i]->p_pgrm == p_pgrm && p_sys->es[i]->b_scrambled )
        {
            b_scrambled = true;
            break;
        }
    }
    if( !p_pgrm->b_scrambled == !b_scrambled )
        return;
    p_pgrm->b_scrambled = b_scrambled;
    char *psz_cat = EsOutProgramGetMetaName( p_pgrm );
    if( b_scrambled )
        input_Control( p_input, INPUT_ADD_INFO, psz_cat, _("Scrambled"), _("Yes") );
    else
        input_Control( p_input, INPUT_DEL_INFO, psz_cat, _("Scrambled") );
    free( psz_cat );
    input_SendEventProgramScrambled( p_input, p_pgrm->i_id, b_scrambled );
}
