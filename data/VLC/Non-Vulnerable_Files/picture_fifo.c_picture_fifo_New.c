}
picture_fifo_t *picture_fifo_New(void)
{
    picture_fifo_t *fifo = malloc(sizeof(*fifo));
    if (!fifo)
        return NULL;
    vlc_mutex_init(&fifo->lock);
    PictureFifoReset(fifo);
    return fifo;
}
