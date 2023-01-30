AVFILTER_DEFINE_CLASS(sidechaincompress);
static av_cold int init(AVFilterContext *ctx)
{
    SidechainCompressContext *s = ctx->priv;
    s->thres = log(s->threshold);
    s->lin_knee_start = s->threshold / sqrt(s->knee);
    s->knee_start = log(s->lin_knee_start);
    s->knee_stop = log(s->threshold * sqrt(s->knee));
    s->compressed_knee_stop = (s->knee_stop - s->thres) / s->ratio + s->thres;
    return 0;
}
