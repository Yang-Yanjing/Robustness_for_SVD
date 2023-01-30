} timestamp_fifo_t;
static timestamp_fifo_t *timestamp_FifoNew(uint32_t capacity)
{
    timestamp_fifo_t *fifo = calloc(1, sizeof(*fifo));
    if (!fifo)
        return NULL;
    fifo->buffer = malloc(capacity * sizeof(*fifo->buffer));
    if (!fifo->buffer) {
        free(fifo);
        return NULL;
    }
    fifo->capacity = capacity;
    return fifo;
}
