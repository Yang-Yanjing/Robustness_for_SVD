static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    StackContext *s = ctx->priv;
    AVRational time_base = ctx->inputs[0]->time_base;
    AVRational frame_rate = ctx->inputs[0]->frame_rate;
    int height = ctx->inputs[0]->h;
    int width = ctx->inputs[0]->w;
    FFFrameSyncIn *in;
    int i, ret;
    if (s->is_vertical) {
        for (i = 1; i < s->nb_inputs; i++) {
            if (ctx->inputs[i]->w != width) {
                av_log(ctx, AV_LOG_ERROR, "Input %d width %d does not match input %d width %d.\n", i, ctx->inputs[i]->w, 0, width);
                return AVERROR(EINVAL);
            }
            height += ctx->inputs[i]->h;
        }
    } else {
        for (i = 1; i < s->nb_inputs; i++) {
            if (ctx->inputs[i]->h != height) {
                av_log(ctx, AV_LOG_ERROR, "Input %d height %d does not match input %d height %d.\n", i, ctx->inputs[i]->h, 0, height);
                return AVERROR(EINVAL);
            }
            width += ctx->inputs[i]->w;
        }
    }
    s->desc = av_pix_fmt_desc_get(outlink->format);
    if (!s->desc)
        return AVERROR_BUG;
    s->nb_planes = av_pix_fmt_count_planes(outlink->format);
    outlink->w          = width;
    outlink->h          = height;
    outlink->time_base  = time_base;
    outlink->frame_rate = frame_rate;
    if ((ret = ff_framesync_init(&s->fs, ctx, s->nb_inputs)) < 0)
        return ret;
    in = s->fs.in;
    s->fs.opaque = s;
    s->fs.on_event = process_frame;
    for (i = 0; i < s->nb_inputs; i++) {
        AVFilterLink *inlink = ctx->inputs[i];
        in[i].time_base = inlink->time_base;
        in[i].sync   = 1;
        in[i].before = EXT_STOP;
        in[i].after  = EXT_INFINITY;
    }
    return ff_framesync_configure(&s->fs);
}
