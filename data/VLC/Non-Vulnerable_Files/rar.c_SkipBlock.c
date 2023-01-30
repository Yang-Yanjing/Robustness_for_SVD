}
static int SkipBlock(stream_t *s, const rar_block_t *hdr)
{
    uint64_t size = (uint64_t)hdr->size + hdr->add_size;
    while (size > 0) {
        int skip = __MIN(size, INT_MAX);
        if (stream_Read(s, NULL, skip) < skip)
            return VLC_EGENERIC;
        size -= skip;
    }
    return VLC_SUCCESS;
}
