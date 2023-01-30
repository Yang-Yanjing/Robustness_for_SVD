 */
static int OpenXZ (vlc_object_t *obj)
{
    stream_t      *stream = (stream_t *)obj;
    const uint8_t *peek;
    /* (Try to) parse the xz stream header */
    if (stream_Peek (stream->p_source, &peek, 8) < 8)
        return VLC_EGENERIC;
    if (memcmp (peek, "\xfd\x37\x7a\x58\x5a", 6))
        return VLC_EGENERIC;
    msg_Dbg (obj, "detected xz compressed stream");
    return Open (stream, "xzcat");
}
