static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    DetelecineContext *s = ctx->priv;
    const AVFilterLink *inlink = ctx->inputs[0];
    AVRational fps = inlink->frame_rate;
    if (!fps.num || !fps.den) {
        av_log(ctx, AV_LOG_ERROR, "The input needs a constant frame rate; "
               "current rate of %d/%d is invalid\n", fps.num, fps.den);
        return AVERROR(EINVAL);
    }
    fps = av_mul_q(fps, av_inv_q(s->pts));
    av_log(ctx, AV_LOG_VERBOSE, "FPS: %d/%d -> %d/%d\n",
           inlink->frame_rate.num, inlink->frame_rate.den, fps.num, fps.den);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    outlink->frame_rate = fps;
    outlink->time_base = av_mul_q(inlink->time_base, s->pts);
    av_log(ctx, AV_LOG_VERBOSE, "TB: %d/%d -> %d/%d\n",
           inlink->time_base.num, inlink->time_base.den, outlink->time_base.num, outlink->time_base.den);
    s->ts_unit = av_inv_q(av_mul_q(fps, outlink->time_base));
    return 0;
}
