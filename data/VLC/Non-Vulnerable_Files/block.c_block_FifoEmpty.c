}
void block_FifoEmpty( block_fifo_t *p_fifo )
{
    block_t *block;
    vlc_mutex_lock( &p_fifo->lock );
    block = p_fifo->p_first;
    if (block != NULL)
    {
        p_fifo->i_depth = p_fifo->i_size = 0;
        p_fifo->p_first = NULL;
        p_fifo->pp_last = &p_fifo->p_first;
    }
    vlc_cond_broadcast( &p_fifo->wait_room );
    vlc_mutex_unlock( &p_fifo->lock );
    while (block != NULL)
    {
        block_t *buf;
        buf = block->p_next;
        block_Release (block);
        block = buf;
    }
}
