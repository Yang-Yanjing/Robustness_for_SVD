static int rtmp_uncompress_swfplayer(uint8_t *in_data, int64_t in_size,
                                     uint8_t **out_data, int64_t *out_size)
{
    z_stream zs = { 0 };
    void *ptr;
    int size;
    int ret = 0;
    zs.avail_in = in_size;
    zs.next_in  = in_data;
    ret = inflateInit(&zs);
    if (ret != Z_OK)
        return AVERROR_UNKNOWN;
    do {
        uint8_t tmp_buf[16384];
        zs.avail_out = sizeof(tmp_buf);
        zs.next_out  = tmp_buf;
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            ret = AVERROR_UNKNOWN;
            goto fail;
        }
        size = sizeof(tmp_buf) - zs.avail_out;
        if (!(ptr = av_realloc(*out_data, *out_size + size))) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        *out_data = ptr;
        memcpy(*out_data + *out_size, tmp_buf, size);
        *out_size += size;
    } while (zs.avail_out == 0);
fail:
    inflateEnd(&zs);
    return ret;
}
