}
static void EsOutProgramsChangeRate( es_out_t *out )
{
    es_out_sys_t      *p_sys = out->p_sys;
    for( int i = 0; i < p_sys->i_pgrm; i++ )
        input_clock_ChangeRate( p_sys->pgrm[i]->p_clock, p_sys->i_rate );
}
