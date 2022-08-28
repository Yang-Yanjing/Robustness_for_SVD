static int expand_text(AVFilterContext *ctx, char *text, AVBPrint *bp)
{
    int ret;
    av_bprint_clear(bp);
    while (*text) {
        if (*text == '\\' && text[1]) {
            av_bprint_chars(bp, text[1], 1);
            text += 2;
        } else if (*text == '%') {
            text++;
            if ((ret = expand_function(ctx, bp, &text)) < 0)
                return ret;
        } else {
            av_bprint_chars(bp, *text, 1);
            text++;
        }
    }
    if (!av_bprint_is_complete(bp))
        return AVERROR(ENOMEM);
    return 0;
}
