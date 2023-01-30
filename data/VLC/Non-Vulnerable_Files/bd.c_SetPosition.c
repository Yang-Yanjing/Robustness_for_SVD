}
static int SetPosition( demux_t *p_demux, double f_position )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const input_title_t *p_title = p_sys->pp_title[p_demux->info.i_title];
    if( p_title->i_length <= 0 )
        return VLC_EGENERIC;
    return SetTime( p_demux, f_position * p_title->i_length );
}
