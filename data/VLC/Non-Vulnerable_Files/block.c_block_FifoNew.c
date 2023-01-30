};
block_fifo_t *block_FifoNew( void )
{
    block_fifo_t *p_fifo = malloc( sizeof( block_fifo_t ) );
    if( !p_fifo )
        return NULL;
    vlc_mutex_init( &p_fifo->lock );
    vlc_cond_init( &p_fifo->wait );
    vlc_cond_init( &p_fifo->wait_room );
    p_fifo->p_first = NULL;
    p_fifo->pp_last = &p_fifo->p_first;
    p_fifo->i_depth = p_fifo->i_size = 0;
    p_fifo->b_force_wake = false;
    return p_fifo;
}
