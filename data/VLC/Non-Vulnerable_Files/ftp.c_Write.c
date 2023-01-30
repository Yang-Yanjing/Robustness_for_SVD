#ifdef ENABLE_SOUT
static ssize_t Write( sout_access_out_t *p_access, block_t *p_buffer )
{
    access_sys_t *p_sys = GET_OUT_SYS(p_access);
    size_t i_write = 0;
    assert( p_sys->data.fd != -1 );
    while( p_buffer != NULL )
    {
        block_t *p_next = p_buffer->p_next;;
        i_write += net_Write( p_access, p_sys->data.fd, p_sys->data.p_vs,
                              p_buffer->p_buffer, p_buffer->i_buffer );
        block_Release( p_buffer );
        p_buffer = p_next;
    }
    return i_write;
}
