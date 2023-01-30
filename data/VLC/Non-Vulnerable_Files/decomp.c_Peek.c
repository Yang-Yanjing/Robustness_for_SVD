 */
static int Peek (stream_t *stream, const uint8_t **pbuf, unsigned int len)
{
    stream_sys_t *sys = stream->p_sys;
    block_t *peeked = sys->peeked;
    size_t curlen;
    if (peeked != NULL)
    {
        curlen = peeked->i_buffer;
        if (curlen < len)
           peeked = block_Realloc (peeked, 0, len);
    }
    else
    {
        curlen = 0;
        peeked = block_Alloc (len);
    }
    sys->peeked = peeked;
    if (unlikely(peeked == NULL))
        return 0;
    while (curlen < len)
    {
        ssize_t val;
        val = net_Read (stream, sys->read_fd, NULL,
                        peeked->p_buffer + curlen, len - curlen, false);
        if (val <= 0)
            break;
        curlen += val;
        peeked->i_buffer = curlen;
    }
    *pbuf = peeked->p_buffer;
    return curlen;
}
