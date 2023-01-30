}
picture_t *picture_fifo_Peek(picture_fifo_t *fifo)
{
    vlc_mutex_lock(&fifo->lock);
    picture_t *picture = fifo->first;
    if (picture)
        picture_Hold(picture);
    vlc_mutex_unlock(&fifo->lock);
    return picture;
}
