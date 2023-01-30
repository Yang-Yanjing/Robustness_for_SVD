void av_bprint_escape(AVBPrint *dstbuf, const char *src, const char *special_chars,
                      enum AVEscapeMode mode, int flags)
{
    const char *src0 = src;
    if (mode == AV_ESCAPE_MODE_AUTO)
        mode = AV_ESCAPE_MODE_BACKSLASH; 
    switch (mode) {
    case AV_ESCAPE_MODE_QUOTE:
        
        av_bprint_chars(dstbuf, '\'', 1);
        for (; *src; src++) {
            if (*src == '\'')
                av_bprintf(dstbuf, "'\\''");
            else
                av_bprint_chars(dstbuf, *src, 1);
        }
        av_bprint_chars(dstbuf, '\'', 1);
        break;
    
    default:
        
        for (; *src; src++) {
            int is_first_last       = src == src0 || !*(src+1);
            int is_ws               = !!strchr(WHITESPACES, *src);
            int is_strictly_special = special_chars && strchr(special_chars, *src);
            int is_special          =
                is_strictly_special || strchr("'\\", *src) ||
                (is_ws && (flags & AV_ESCAPE_FLAG_WHITESPACE));
            if (is_strictly_special ||
                (!(flags & AV_ESCAPE_FLAG_STRICT) &&
                 (is_special || (is_ws && is_first_last))))
                av_bprint_chars(dstbuf, '\\', 1);
            av_bprint_chars(dstbuf, *src, 1);
        }
        break;
    }
}
