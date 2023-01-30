}
void block_FifoWake( block_fifo_t *p_fifo )
{
    vlc_mutex_lock( &p_fifo->lock );
    if( p_fifo->p_first == NULL )
        p_fifo->b_force_wake = true;
    vlc_cond_broadcast( &p_fifo->wait );
    vlc_mutex_unlock( &p_fifo->lock );
}
