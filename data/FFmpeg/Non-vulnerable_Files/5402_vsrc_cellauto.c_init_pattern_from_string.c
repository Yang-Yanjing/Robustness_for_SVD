static int init_pattern_from_string(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    char *p;
    int i, w = 0;
    w = strlen(cellauto->pattern);
    av_log(ctx, AV_LOG_DEBUG, "w:%d\n", w);
    if (cellauto->w) {
        if (w > cellauto->w) {
            av_log(ctx, AV_LOG_ERROR,
                   "The specified width is %d which cannot contain the provided string width of %d\n",
                   cellauto->w, w);
            return AVERROR(EINVAL);
        }
    } else {
        
        cellauto->w = w;
        cellauto->h = (double)cellauto->w * M_PHI;
    }
    cellauto->buf = av_mallocz_array(sizeof(uint8_t) * cellauto->w, cellauto->h);
    if (!cellauto->buf)
        return AVERROR(ENOMEM);
    
    p = cellauto->pattern;
    for (i = (cellauto->w - w)/2;; i++) {
        av_log(ctx, AV_LOG_DEBUG, "%d %c\n", i, *p == '\n' ? 'N' : *p);
        if (*p == '\n' || !*p)
            break;
        else
            cellauto->buf[i] = !!av_isgraph(*(p++));
    }
    return 0;
}
