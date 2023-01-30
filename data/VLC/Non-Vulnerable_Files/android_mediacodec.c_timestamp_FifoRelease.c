}
static void timestamp_FifoRelease(timestamp_fifo_t *fifo)
{
    free(fifo->buffer);
    free(fifo);
}
