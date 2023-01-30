}
static void EsOutProgramChangePause( es_out_t *out, bool b_paused, mtime_t i_date )
{
    es_out_sys_t *p_sys = out->p_sys;
    for( int i = 0; i < p_sys->i_pgrm; i++ )
        input_clock_ChangePause( p_sys->pgrm[i]->p_clock, b_paused, i_date );
}
