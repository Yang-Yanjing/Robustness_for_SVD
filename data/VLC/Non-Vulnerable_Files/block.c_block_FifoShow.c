 */
block_t *block_FifoShow( block_fifo_t *p_fifo )
{
    block_t *b;
    vlc_testcancel( );
    vlc_mutex_lock( &p_fifo->lock );
    mutex_cleanup_push( &p_fifo->lock );
    while( p_fifo->p_first == NULL )
        vlc_cond_wait( &p_fifo->wait, &p_fifo->lock );
    b = p_fifo->p_first;
    vlc_cleanup_run ();
    return b;
}
