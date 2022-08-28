static const char *flat_escape_key_str(AVBPrint *dst, const char *src, const char sep)
{
    const char *p;
    for (p = src; *p; p++) {
        if (!((*p >= '0' && *p <= '9') ||
              (*p >= 'a' && *p <= 'z') ||
              (*p >= 'A' && *p <= 'Z')))
            av_bprint_chars(dst, '_', 1);
        else
            av_bprint_chars(dst, *p, 1);
    }
    return dst->str;
}
