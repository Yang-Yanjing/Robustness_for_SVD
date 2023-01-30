static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    RandomContext *s = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled && s->nb_frames > 0) {
        AVFrame *out = s->frames[s->nb_frames - 1];
        out->pts = s->pts[s->flush_idx++];
        ret = ff_filter_frame(outlink, out);
        s->frames[s->nb_frames - 1] = NULL;
        s->nb_frames--;
    }
    return ret;
}
