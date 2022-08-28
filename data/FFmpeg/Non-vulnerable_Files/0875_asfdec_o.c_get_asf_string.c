static int get_asf_string(AVIOContext *pb, int maxlen, char *buf, int buflen)
{
    char *q = buf;
    int ret = 0;
    if (buflen <= 0)
        return AVERROR(EINVAL);
    while (ret + 1 < maxlen) {
        uint8_t tmp;
        uint32_t ch;
        GET_UTF16(ch, (ret += 2) <= maxlen ? avio_rl16(pb) : 0, break;);
        PUT_UTF8(ch, tmp, if (q - buf < buflen - 1) *q++ = tmp;)
    }
    *q = 0;
    return ret;
}
