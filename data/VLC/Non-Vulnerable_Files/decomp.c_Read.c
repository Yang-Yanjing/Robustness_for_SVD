 */
static int Read (stream_t *stream, void *buf, unsigned int buflen)
{
    stream_sys_t *sys = stream->p_sys;
    unsigned ret = 0;
    if (buf == NULL) /* caller skips data, get big enough peek buffer */
        buflen = Peek (stream, &(const uint8_t *){ NULL }, buflen);
    block_t *peeked = sys->peeked;
    if (peeked != NULL)
    {   /* dequeue peeked data */
        size_t length = peeked->i_buffer;
        if (length > buflen)
            length = buflen;
        if (buf != NULL)
        {
            memcpy (buf, peeked->p_buffer, length);
            buf = ((char *)buf) + length;
        }
        buflen -= length;
        peeked->p_buffer += length;
        peeked->i_buffer -= length;
        if (peeked->i_buffer == 0)
        {
            block_Release (peeked);
            sys->peeked = NULL;
        }
        sys->offset += length;
        ret += length;
    }
    assert ((buf != NULL) || (buflen == 0));
    ssize_t val = net_Read (stream, sys->read_fd, NULL, buf, buflen, false);
    if (val > 0)
    {
        sys->offset += val;
        ret += val;
    }
    return ret;
}
