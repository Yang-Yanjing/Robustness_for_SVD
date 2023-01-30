static int func_pts(AVFilterContext *ctx, AVBPrint *bp,
                    char *fct, unsigned argc, char **argv, int tag)
{
    DrawTextContext *s = ctx->priv;
    const char *fmt;
    double pts = s->var_values[VAR_T];
    int ret;
    fmt = argc >= 1 ? argv[0] : "flt";
    if (argc >= 2) {
        int64_t delta;
        if ((ret = av_parse_time(&delta, argv[1], 1)) < 0) {
            av_log(ctx, AV_LOG_ERROR, "Invalid delta '%s'\n", argv[1]);
            return ret;
        }
        pts += (double)delta / AV_TIME_BASE;
    }
    if (!strcmp(fmt, "flt")) {
        av_bprintf(bp, "%.6f", s->var_values[VAR_T]);
    } else if (!strcmp(fmt, "hms")) {
        if (isnan(pts)) {
            av_bprintf(bp, " ??:??:??.???");
        } else {
            int64_t ms = round(pts * 1000);
            char sign = ' ';
            if (ms < 0) {
                sign = '-';
                ms = -ms;
            }
            av_bprintf(bp, "%c%02d:%02d:%02d.%03d", sign,
                       (int)(ms / (60 * 60 * 1000)),
                       (int)(ms / (60 * 1000)) % 60,
                       (int)(ms / 1000) % 60,
                       (int)ms % 1000);
        }
    } else {
        av_log(ctx, AV_LOG_ERROR, "Invalid format '%s'\n", fmt);
        return AVERROR(EINVAL);
    }
    return 0;
}
