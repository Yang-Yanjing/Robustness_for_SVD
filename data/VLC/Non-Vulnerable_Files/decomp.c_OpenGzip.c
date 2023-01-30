 */
static int OpenGzip (vlc_object_t *obj)
{
    stream_t      *stream = (stream_t *)obj;
    const uint8_t *peek;
    if (stream_Peek (stream->p_source, &peek, 3) < 3)
        return VLC_EGENERIC;
    if (memcmp (peek, "\x1f\x8b\x08", 3))
        return VLC_EGENERIC;
    msg_Dbg (obj, "detected gzip compressed stream");
    return Open (stream, "zcat");
}
