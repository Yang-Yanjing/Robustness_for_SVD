}
static int ControlInternal( demux_t *p_demux, int i_query, ... )
{
    va_list args;
    int     i_result;
    va_start( args, i_query );
    i_result = Control( p_demux, i_query, args );
    va_end( args );
    return i_result;
}
