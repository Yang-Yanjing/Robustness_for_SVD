/* */
static block_t *LoadBlock( demux_t *p_demux, const char *psz_name )
{
    stream_t *s = stream_UrlNew( p_demux, psz_name );
    if( !s )
        return NULL;
    const int64_t i_size = stream_Size( s );
    block_t *p_block = NULL;
    if( i_size > 0 && i_size < INT_MAX )
        p_block = stream_Block( s, i_size );
    stream_Delete( s );
    return p_block;
}
