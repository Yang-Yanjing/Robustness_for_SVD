 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    return demux_vaControlHelper( p_demux->s, 0, -1,
                                   p_sys->fmt.i_bitrate, p_sys->i_seek_step, i_query, args );
}
