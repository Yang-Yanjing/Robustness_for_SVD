 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    /* XXX: DEMUX_SET_TIME is precise here */
    return demux_vaControlHelper( p_demux->s,
                                   0, -1,
                                   p_sys->frame_size * p_sys->f_rate * 8,
                                   p_sys->frame_size, i_query, args );
}
