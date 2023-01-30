 */
static es_out_pgrm_t *EsOutProgramFind( es_out_t *p_out, int i_group )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    for( int i = 0; i < p_sys->i_pgrm; i++ )
    {
        if( p_sys->pgrm[i]->i_id == i_group )
            return p_sys->pgrm[i];
    }
    return EsOutProgramAdd( p_out, i_group );
}
