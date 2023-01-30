}
static void EsOutChangeRate( es_out_t *out, int i_rate )
{
    es_out_sys_t      *p_sys = out->p_sys;
    p_sys->i_rate = i_rate;
    EsOutProgramsChangeRate( out );
}
