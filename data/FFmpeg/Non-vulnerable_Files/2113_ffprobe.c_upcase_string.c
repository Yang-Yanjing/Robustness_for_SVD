static inline char *upcase_string(char *dst, size_t dst_size, const char *src)
{
    int i;
    for (i = 0; src[i] && i < dst_size-1; i++)
        dst[i] = av_toupper(src[i]);
    dst[i] = 0;
    return dst;
}
