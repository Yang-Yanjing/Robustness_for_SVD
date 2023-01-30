 */
size_t block_FifoPut( block_fifo_t *p_fifo, block_t *p_block )
{
    size_t i_size = 0, i_depth = 0;
    block_t *p_last;
    if (p_block == NULL)
        return 0;
    for (p_last = p_block; ; p_last = p_last->p_next)
    {
        i_size += p_last->i_buffer;
        i_depth++;
        if (!p_last->p_next)
            break;
    }
    vlc_mutex_lock (&p_fifo->lock);
    *p_fifo->pp_last = p_block;
    p_fifo->pp_last = &p_last->p_next;
    p_fifo->i_depth += i_depth;
    p_fifo->i_size += i_size;
    /* We queued at least one block: wake up one read-waiting thread */
    vlc_cond_signal( &p_fifo->wait );
    vlc_mutex_unlock( &p_fifo->lock );
    return i_size;
}
