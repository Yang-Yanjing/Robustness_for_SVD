 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
     /* (2**31)-1 is insufficient to store 1080p50 4:4:4. */
    const int64_t i_bps = 8LL * p_sys->frame_size * p_sys->pcr.i_divider_num /
                                                    p_sys->pcr.i_divider_den;
    /* XXX: DEMUX_SET_TIME is precise here */
    return demux_vaControlHelper( p_demux->s, 0, -1, i_bps,
                                   p_sys->frame_size, i_query, args );
}
