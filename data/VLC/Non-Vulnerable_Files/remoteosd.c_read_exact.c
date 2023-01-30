}
static bool read_exact( filter_t *p_filter,
                        int i_socket,
                        char* p_readbuf,
                        int i_bytes )
{
    return i_bytes == net_Read( p_filter, i_socket, NULL,
                                  (unsigned char*)p_readbuf,
                                  i_bytes, true );
}
