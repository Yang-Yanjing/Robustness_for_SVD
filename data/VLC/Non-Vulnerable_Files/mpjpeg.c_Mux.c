}
static int Mux( sout_mux_t *p_mux )
{
    block_fifo_t *p_fifo;
    if( !p_mux->i_nb_inputs ) return VLC_SUCCESS;
    p_fifo = p_mux->pp_inputs[0]->p_fifo;
    while( block_FifoCount( p_fifo ) > 0 )
    {
        static const char psz_hfmt[] = "\r\n"
            "--"BOUNDARY"\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: %zu\r\n"
            "\r\n";
        block_t *p_data = block_FifoGet( p_fifo );
        block_t *p_header = block_Alloc( sizeof( psz_hfmt ) + 20 );
        if( p_header == NULL ) /* uho! */
        {
            block_Release( p_data );
            continue;
        }
        p_header->i_buffer =
            snprintf( (char *)p_header->p_buffer, p_header->i_buffer,
                      psz_hfmt, p_data->i_buffer );
        p_header->i_flags |= BLOCK_FLAG_HEADER;
        sout_AccessOutWrite( p_mux->p_access, p_header );
        sout_AccessOutWrite( p_mux->p_access, p_data );
    }
    return VLC_SUCCESS;
}
