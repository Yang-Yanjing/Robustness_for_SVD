}
static void timestamp_FifoPut(timestamp_fifo_t *fifo, int64_t ts)
{
    uint32_t end = (fifo->begin + fifo->size) % fifo->capacity;
    fifo->buffer[end] = ts;
    if (!timestamp_FifoIsFull(fifo))
        fifo->size += 1;
    else
        fifo->begin = (fifo->begin + 1) % fifo->capacity;
}
