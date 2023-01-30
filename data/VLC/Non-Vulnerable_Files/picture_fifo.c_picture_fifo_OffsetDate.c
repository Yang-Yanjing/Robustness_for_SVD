}
void picture_fifo_OffsetDate(picture_fifo_t *fifo, mtime_t delta)
{
    vlc_mutex_lock(&fifo->lock);
    for (picture_t *picture = fifo->first; picture != NULL;) {
        picture->date += delta;
        picture = picture->p_next;
    }
    vlc_mutex_unlock(&fifo->lock);
}
