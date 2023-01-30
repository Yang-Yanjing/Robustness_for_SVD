static int mkv_strip_wavpack(const uint8_t *src, uint8_t **pdst, int *size)
{
    uint8_t *dst;
    int srclen = *size;
    int offset = 0;
    int ret;
    dst = av_malloc(srclen);
    if (!dst)
        return AVERROR(ENOMEM);
    while (srclen >= WV_HEADER_SIZE) {
        WvHeader header;
        ret = ff_wv_parse_header(&header, src);
        if (ret < 0)
            goto fail;
        src    += WV_HEADER_SIZE;
        srclen -= WV_HEADER_SIZE;
        if (srclen < header.blocksize) {
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        if (header.initial) {
            AV_WL32(dst + offset, header.samples);
            offset += 4;
        }
        AV_WL32(dst + offset,     header.flags);
        AV_WL32(dst + offset + 4, header.crc);
        offset += 8;
        if (!(header.initial && header.final)) {
            AV_WL32(dst + offset, header.blocksize);
            offset += 4;
        }
        memcpy(dst + offset, src, header.blocksize);
        src    += header.blocksize;
        srclen -= header.blocksize;
        offset += header.blocksize;
    }
    *pdst = dst;
    *size = offset;
    return 0;
fail:
    av_freep(&dst);
    return ret;
}
