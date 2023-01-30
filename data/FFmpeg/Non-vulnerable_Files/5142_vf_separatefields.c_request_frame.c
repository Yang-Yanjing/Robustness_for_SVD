static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SeparateFieldsContext *s = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && s->second) {
        s->second->pts *= 2;
        extract_field(s->second, s->nb_planes, s->second->top_field_first);
        ret = ff_filter_frame(outlink, s->second);
        s->second = 0;
    }
    return ret;
}
