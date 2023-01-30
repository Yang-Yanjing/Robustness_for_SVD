}
static gme_err_t ReaderStream (void *data, void *buf, int length)
{
    stream_t *s = data;
    if (stream_Read (s, buf, length) < length)
        return "short read";
    return NULL;
}
