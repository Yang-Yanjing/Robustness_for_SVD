}
static bool timestamp_FifoIsFull(timestamp_fifo_t *fifo)
{
    return fifo->size == fifo->capacity;
}
