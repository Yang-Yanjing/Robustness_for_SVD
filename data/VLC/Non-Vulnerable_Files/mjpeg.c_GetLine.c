 *****************************************************************************/
static char* GetLine( demux_t *p_demux, int *p_pos )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const uint8_t *p_buf;
    int         i_size;
    int         i;
    char        *p_line;
    while( *p_pos >= p_sys->i_data_peeked )
    {
        if( ! Peek( p_demux, false ) )
        {
            return NULL;
        }
    }
    p_buf = p_sys->p_peek + *p_pos;
    i_size = p_sys->i_data_peeked - *p_pos;
    i = 0;
    while( p_buf[i] != '\n' )
    {
        i++;
        if( i == i_size )
        {
            if( ! Peek( p_demux, false ) )
            {
                return NULL;
            }
            p_buf = p_sys->p_peek + *p_pos;
            i_size = p_sys->i_data_peeked - *p_pos;
        }
    }
    *p_pos += i + 1;
    if( i > 0 && p_buf[i - 1] == '\r' )
    {
        i--;
    }
    p_line = malloc( i + 1 );
    if( unlikely( p_line == NULL ) )
        return NULL;
    strncpy ( p_line, (char*)p_buf, i );
    p_line[i] = '\0';
    return p_line;
}
