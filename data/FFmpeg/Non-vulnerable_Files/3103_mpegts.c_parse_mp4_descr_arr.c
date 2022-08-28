                           int target_tag);
static int parse_mp4_descr_arr(MP4DescrParseContext *d, int64_t off, int len)
{
    while (len > 0) {
        int ret = parse_mp4_descr(d, off, len, 0);
        if (ret < 0)
            return ret;
        update_offsets(&d->pb, &off, &len);
    }
    return 0;
}
