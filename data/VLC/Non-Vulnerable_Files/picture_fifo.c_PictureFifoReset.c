};
static void PictureFifoReset(picture_fifo_t *fifo)
{
    fifo->first    = NULL;
    fifo->last_ptr = &fifo->first;
}
