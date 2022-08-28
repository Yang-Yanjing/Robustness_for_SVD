static int mp4_read_iods(AVFormatContext *s, const uint8_t *buf, unsigned size,
                         Mp4Descr *descr, int *descr_count, int max_descr_count)
{
    MP4DescrParseContext d;
    int ret;
    ret = init_MP4DescrParseContext(&d, s, buf, size, descr, max_descr_count);
    if (ret < 0)
        return ret;
    ret = parse_mp4_descr(&d, avio_tell(&d.pb), size, MP4IODescrTag);
    *descr_count = d.descr_count;
    return ret;
}
