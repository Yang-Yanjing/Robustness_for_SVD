static av_cold int init(AVFilterContext *ctx)
{
    DelogoContext *s = ctx->priv;
#define CHECK_UNSET_OPT(opt)                                            \
    if (s->opt == -1) {                                            \
        av_log(s, AV_LOG_ERROR, "Option %s was not set.\n", #opt); \
        return AVERROR(EINVAL);                                         \
    }
    CHECK_UNSET_OPT(x);
    CHECK_UNSET_OPT(y);
    CHECK_UNSET_OPT(w);
    CHECK_UNSET_OPT(h);
    av_log(ctx, AV_LOG_VERBOSE, "x:%d y:%d, w:%d h:%d band:%d show:%d\n",
           s->x, s->y, s->w, s->h, s->band, s->show);
    s->w += s->band*2;
    s->h += s->band*2;
    s->x -= s->band;
    s->y -= s->band;
    return 0;
}
