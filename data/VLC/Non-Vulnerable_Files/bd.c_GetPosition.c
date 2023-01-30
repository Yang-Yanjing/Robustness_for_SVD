}
static double GetPosition( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const int64_t i_time = GetTime( p_demux );
    const input_title_t *p_title = p_sys->pp_title[p_demux->info.i_title];
    if( p_title->i_length <= 0 )
        return 0.0;
    return (double)i_time / p_title->i_length;
}
