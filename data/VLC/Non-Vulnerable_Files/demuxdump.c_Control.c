}
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    return demux_vaControlHelper( p_demux->s, 0, -1, 0, 1, i_query, args );
}
