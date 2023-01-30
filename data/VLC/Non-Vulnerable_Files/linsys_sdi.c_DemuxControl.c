 *****************************************************************************/
static int DemuxControl( demux_t *p_demux, int i_query, va_list args )
{
    return demux_vaControlHelper( p_demux->s, -1, -1, 270000000, 1, i_query,
                                   args );
}
