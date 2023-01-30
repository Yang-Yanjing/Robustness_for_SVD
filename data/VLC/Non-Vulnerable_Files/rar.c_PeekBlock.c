};
static int PeekBlock(stream_t *s, rar_block_t *hdr)
{
    const uint8_t *peek;
    int peek_size = stream_Peek(s, &peek, 11);
    if (peek_size < 7)
        return VLC_EGENERIC;
    hdr->crc   = GetWLE(&peek[0]);
    hdr->type  = peek[2];
    hdr->flags = GetWLE(&peek[3]);
    hdr->size  = GetWLE(&peek[5]);
    hdr->add_size = 0;
    if ((hdr->flags & 0x8000) ||
        hdr->type == RAR_BLOCK_FILE ||
        hdr->type == RAR_BLOCK_SUBBLOCK) {
        if (peek_size < 11)
            return VLC_EGENERIC;
        hdr->add_size = GetDWLE(&peek[7]);
    }
    if (hdr->size < 7)
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
