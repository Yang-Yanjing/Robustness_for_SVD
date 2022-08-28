static uint64_t mxf_utf16len(const char *utf8_str)
{
    const uint8_t *q = utf8_str;
    uint64_t size = 0;
    while (*q) {
        uint32_t ch;
        GET_UTF8(ch, *q++, goto invalid;)
        if (ch < 0x10000)
            size++;
        else
            size += 2;
        continue;
invalid:
        av_log(NULL, AV_LOG_ERROR, "Invaid UTF8 sequence in mxf_utf16len\n\n");
    }
    size += 1;
    return size;
}
