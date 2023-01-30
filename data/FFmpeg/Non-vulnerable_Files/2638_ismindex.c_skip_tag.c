static int skip_tag(AVIOContext *in, int32_t tag_name)
{
    int64_t pos = avio_tell(in);
    int32_t size, tag;
    size = avio_rb32(in);
    tag  = avio_rb32(in);
    if (expect_tag(tag, tag_name) != 0)
        return -1;
    avio_seek(in, pos + size, SEEK_SET);
    return 0;
}
