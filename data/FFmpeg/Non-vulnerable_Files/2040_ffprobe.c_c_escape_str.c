static const char *c_escape_str(AVBPrint *dst, const char *src, const char sep, void *log_ctx)
{
    const char *p;
    for (p = src; *p; p++) {
        switch (*p) {
        case '\b': av_bprintf(dst, "%s", "\\b");  break;
        case '\f': av_bprintf(dst, "%s", "\\f");  break;
        case '\n': av_bprintf(dst, "%s", "\\n");  break;
        case '\r': av_bprintf(dst, "%s", "\\r");  break;
        case '\\': av_bprintf(dst, "%s", "\\\\"); break;
        default:
            if (*p == sep)
                av_bprint_chars(dst, '\\', 1);
            av_bprint_chars(dst, *p, 1);
        }
    }
    return dst->str;
}
