}
void block_FifoRelease( block_fifo_t *p_fifo )
{
    block_FifoEmpty( p_fifo );
    vlc_cond_destroy( &p_fifo->wait_room );
    vlc_cond_destroy( &p_fifo->wait );
    vlc_mutex_destroy( &p_fifo->lock );
    free( p_fifo );
}
