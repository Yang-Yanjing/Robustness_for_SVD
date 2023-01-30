                                  int64_t *pos_arg, int64_t pos_limit);
static int get_str(AVIOContext *bc, char *string, unsigned int maxlen)
{
    unsigned int len = ffio_read_varlen(bc);
    if (len && maxlen)
        avio_read(bc, string, FFMIN(len, maxlen));
    while (len > maxlen) {
        avio_r8(bc);
        len--;
        if (bc->eof_reached)
            len = maxlen;
    }
    if (maxlen)
        string[FFMIN(len, maxlen - 1)] = 0;
    if (bc->eof_reached)
        return AVERROR_EOF;
    if (maxlen == len)
        return -1;
    else
        return 0;
}
