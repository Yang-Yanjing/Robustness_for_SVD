}
static bool write_exact( filter_t *p_filter,
                         int i_socket,
                         char* p_writebuf,
                         int i_bytes )
{
    return i_bytes == net_Write( p_filter, i_socket, NULL,
                                  (unsigned char*)p_writebuf, i_bytes );
}
