 */
void block_FifoPace (block_fifo_t *fifo, size_t max_depth, size_t max_size)
{
    vlc_testcancel ();
    vlc_mutex_lock (&fifo->lock);
    while ((fifo->i_depth > max_depth) || (fifo->i_size > max_size))
    {
         mutex_cleanup_push (&fifo->lock);
         vlc_cond_wait (&fifo->wait_room, &fifo->lock);
         vlc_cleanup_pop ();
    }
    vlc_mutex_unlock (&fifo->lock);
}
