}
void bd_clpi_stream_Parse( bd_clpi_stream_t *p_stream, bs_t *s )
{
    p_stream->i_pid = bs_read( s, 16 );
    const int i_length = bs_read( s, 8 );
    p_stream->i_type = bs_read( s, 8 );
    /* Ignore the rest */
    if( i_length > 1 )
        bs_skip( s, 8*i_length - 8 );
}
