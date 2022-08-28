static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    DynamicAudioNormalizerContext *s = ctx->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    int ret = 0;
    if (!cqueue_empty(s->gain_history_smoothed[0])) {
        AVFrame *out = ff_bufqueue_get(&s->queue);
        amplify_frame(s, out);
        ret = ff_filter_frame(outlink, out);
    }
    analyze_frame(s, in);
    ff_bufqueue_add(ctx, &s->queue, in);
    return ret;
}
