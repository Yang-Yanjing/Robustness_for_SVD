static int get_moov_size(AVFormatContext *s);
static int utf8len(const uint8_t *b)
{
    int len = 0;
    int val;
    while (*b) {
        GET_UTF8(val, *b++, return -1;)
        len++;
    }
    return len;
}
