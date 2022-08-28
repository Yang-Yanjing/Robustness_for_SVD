static int mp4_read_od(AVFormatContext *s, const uint8_t *buf, unsigned size,
                       Mp4Descr *descr, int *descr_count, int max_descr_count)
{
    MP4DescrParseContext d;
    int ret;
    ret = init_MP4DescrParseContext(&d, s, buf, size, descr, max_descr_count);
    if (ret < 0)
        return ret;
    ret = parse_mp4_descr_arr(&d, avio_tell(&d.pb), size);
    *descr_count = d.descr_count;
    return ret;
}
