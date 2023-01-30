 */
static int OpenBzip2 (vlc_object_t *obj)
{
    stream_t      *stream = (stream_t *)obj;
    const uint8_t *peek;
    /* (Try to) parse the bzip2 header */
    if (stream_Peek (stream->p_source, &peek, 10) < 10)
        return VLC_EGENERIC;
    if (memcmp (peek, "BZh", 3) || (peek[3] < '1') || (peek[3] > '9')
     || memcmp (peek + 4, "\x31\x41\x59\x26\x53\x59", 6))
        return VLC_EGENERIC;
    msg_Dbg (obj, "detected bzip2 compressed stream");
    return Open (stream, "bzcat");
}
