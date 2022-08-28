AVFILTER_DEFINE_CLASS(detelecine);
static av_cold int init(AVFilterContext *ctx)
{
    DetelecineContext *s = ctx->priv;
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
        s->pts.num += *p - '0';
        s->pts.den += 2;
    }
    s->nskip_fields = 0;
    s->pattern_pos = 0;
    s->start_time = AV_NOPTS_VALUE;
    if (s->start_frame != 0) {
        int nfields = 0;
        for (p = s->pattern; *p; p++) {
            nfields += *p - '0';
            s->pattern_pos++;
            if (nfields >= 2*s->start_frame) {
                s->nskip_fields = nfields - 2*s->start_frame;
                break;
            }
        }
    }
    av_log(ctx, AV_LOG_INFO, "Detelecine pattern %s removes up to %d frames per frame, pts advance factor: %d/%d\n",
           s->pattern, (max + 1) / 2, s->pts.num, s->pts.den);
    return 0;
}
