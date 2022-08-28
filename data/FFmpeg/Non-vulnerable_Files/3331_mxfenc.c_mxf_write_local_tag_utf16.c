static void mxf_write_local_tag_utf16(AVIOContext *pb, int tag, const char *value)
{
    uint64_t size = mxf_utf16len(value);
    if (size >= UINT16_MAX/2) {
        av_log(NULL, AV_LOG_ERROR, "utf16 local tag size %"PRIx64" invalid (too large), ignoring\n", size);
        return;
    }
    mxf_write_local_tag(pb, size*2, tag);
    avio_put_str16be(pb, value);
}
