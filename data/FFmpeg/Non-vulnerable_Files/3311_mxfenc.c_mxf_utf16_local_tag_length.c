static int mxf_utf16_local_tag_length(const char *utf8_str)
{
    uint64_t size;
    if (!utf8_str)
        return 0;
    size = mxf_utf16len(utf8_str);
    if (size >= UINT16_MAX/2) {
        av_log(NULL, AV_LOG_ERROR, "utf16 local tag size %"PRIx64" invalid (too large), ignoring\n", size);
        return 0;
    }
    return 4 + size * 2;
}
