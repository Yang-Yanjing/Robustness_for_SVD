 */
block_t *block_FifoGet( block_fifo_t *p_fifo )
{
    block_t *b;
    vlc_testcancel( );
    vlc_mutex_lock( &p_fifo->lock );
    mutex_cleanup_push( &p_fifo->lock );
    /* Remember vlc_cond_wait() may cause spurious wakeups
     * (on both Win32 and POSIX) */
    while( ( p_fifo->p_first == NULL ) && !p_fifo->b_force_wake )
        vlc_cond_wait( &p_fifo->wait, &p_fifo->lock );
    vlc_cleanup_pop();
    b = p_fifo->p_first;
    p_fifo->b_force_wake = false;
    if( b == NULL )
    {
        /* Forced wakeup */
        vlc_mutex_unlock( &p_fifo->lock );
        return NULL;
    }
    p_fifo->p_first = b->p_next;
    p_fifo->i_depth--;
    p_fifo->i_size -= b->i_buffer;
    if( p_fifo->p_first == NULL )
    {
        p_fifo->pp_last = &p_fifo->p_first;
    }
    /* We don't know how many threads can queue new packets now. */
    vlc_cond_broadcast( &p_fifo->wait_room );
    vlc_mutex_unlock( &p_fifo->lock );
    b->p_next = NULL;
    return b;
}
