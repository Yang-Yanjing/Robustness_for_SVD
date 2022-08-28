static int copy_tag(AVIOContext *in, AVIOContext *out, int32_t tag_name)
{
    int32_t size, tag;
    size = avio_rb32(in);
    tag  = avio_rb32(in);
    avio_wb32(out, size);
    avio_wb32(out, tag);
    if (expect_tag(tag, tag_name) != 0)
        return -1;
    size -= 8;
    while (size > 0) {
        char buf[1024];
        int len = FFMIN(sizeof(buf), size);
        int got;
        if ((got = avio_read(in, buf, len)) != len) {
            fprintf(stderr, "short read, wanted %d, got %d\n", len, got);
            break;
        }
        avio_write(out, buf, len);
        size -= len;
    }
    return 0;
}
