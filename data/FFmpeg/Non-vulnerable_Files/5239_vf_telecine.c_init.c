AVFILTER_DEFINE_CLASS(telecine);
static av_cold int init(AVFilterContext *ctx)
{
    TelecineContext *s = ctx->priv;
    const char *p;
    int max = 0;
    if (!strlen(s->pattern)) {
        av_log(ctx, AV_LOG_ERROR, "No pattern provided.\n");
        return AVERROR_INVALIDDATA;
    }
    for (p = s->pattern; *p; p++) {
        if (!av_isdigit(*p)) {
            av_log(ctx, AV_LOG_ERROR, "Provided pattern includes non-numeric characters.\n");
            return AVERROR_INVALIDDATA;
        }
        max = FFMAX(*p - '0', max);
        s->pts.num += 2;
        s->pts.den += *p - '0';
    }
    s->start_time = AV_NOPTS_VALUE;
    s->out_cnt = (max + 1) / 2;
    av_log(ctx, AV_LOG_INFO, "Telecine pattern %s yields up to %d frames per frame, pts advance factor: %d/%d\n",
           s->pattern, s->out_cnt, s->pts.num, s->pts.den);
    return 0;
}
