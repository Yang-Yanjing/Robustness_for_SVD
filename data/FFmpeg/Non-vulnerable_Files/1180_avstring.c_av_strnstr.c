char *av_strnstr(const char *haystack, const char *needle, size_t hay_length)
{
    size_t needle_len = strlen(needle);
    if (!needle_len)
        return (char*)haystack;
    while (hay_length >= needle_len) {
        hay_length--;
        if (!memcmp(haystack, needle, needle_len))
            return (char*)haystack;
        haystack++;
    }
    return NULL;
}
