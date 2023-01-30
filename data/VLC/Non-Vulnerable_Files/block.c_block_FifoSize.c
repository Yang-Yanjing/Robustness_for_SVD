/* FIXME: not thread-safe */
size_t block_FifoSize( const block_fifo_t *p_fifo )
{
    return p_fifo->i_size;
}
