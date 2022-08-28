static const char *xml_escape_str(AVBPrint *dst, const char *src, void *log_ctx)
{
    const char *p;
    for (p = src; *p; p++) {
        switch (*p) {
        case '&' : av_bprintf(dst, "%s", "&amp;");  break;
        case '<' : av_bprintf(dst, "%s", "&lt;");   break;
        case '>' : av_bprintf(dst, "%s", "&gt;");   break;
        case '"' : av_bprintf(dst, "%s", "&quot;"); break;
        case '\'': av_bprintf(dst, "%s", "&apos;"); break;
        default: av_bprint_chars(dst, *p, 1);
        }
    }
    return dst->str;
}
