AVFILTER_DEFINE_CLASS(dejudder);
static int config_out_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    DejudderContext *s = ctx->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    outlink->time_base = av_mul_q(inlink->time_base, av_make_q(1, 2 * s->cycle));
    outlink->frame_rate = av_mul_q(inlink->frame_rate, av_make_q(2 * s->cycle, 1));
    av_log(ctx, AV_LOG_VERBOSE, "cycle:%d\n", s->cycle);
    return 0;
}
