}
picture_t *picture_fifo_Pop(picture_fifo_t *fifo)
{
    vlc_mutex_lock(&fifo->lock);
    picture_t *picture = PictureFifoPop(fifo);
    vlc_mutex_unlock(&fifo->lock);
    return picture;
}
