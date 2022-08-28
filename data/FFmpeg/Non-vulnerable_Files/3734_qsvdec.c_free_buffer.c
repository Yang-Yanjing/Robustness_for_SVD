static void free_buffer(void *opaque, uint8_t *data)
{
    int *used = opaque;
    *used = 0;
    av_freep(&data);
}
