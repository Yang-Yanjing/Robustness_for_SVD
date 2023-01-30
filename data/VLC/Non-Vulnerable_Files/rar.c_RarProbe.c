}
int RarProbe(stream_t *s)
{
    const uint8_t *peek;
    if (stream_Peek(s, &peek, rar_marker_size) < rar_marker_size)
        return VLC_EGENERIC;
    if (memcmp(peek, rar_marker, rar_marker_size))
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
