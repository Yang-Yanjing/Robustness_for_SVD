}
static int SkipEnd(stream_t *s, const rar_block_t *hdr)
{
    if (!(hdr->flags & RAR_BLOCK_END_HAS_NEXT))
        return VLC_EGENERIC;
    if (SkipBlock(s, hdr))
        return VLC_EGENERIC;
    /* Now, we need to look for a marker block,
     * It seems that there is garbage at EOF */
    for (;;) {
        const uint8_t *peek;
        if (stream_Peek(s, &peek, rar_marker_size) < rar_marker_size)
            return VLC_EGENERIC;
        if (!memcmp(peek, rar_marker, rar_marker_size))
            break;
        if (stream_Read(s, NULL, 1) != 1)
            return VLC_EGENERIC;
    }
    /* Skip marker and archive blocks */
    if (IgnoreBlock(s, RAR_BLOCK_MARKER))
        return VLC_EGENERIC;
    if (IgnoreBlock(s, RAR_BLOCK_ARCHIVE))
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
