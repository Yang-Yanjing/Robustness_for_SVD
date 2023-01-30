static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    DecimateContext *dm = ctx->priv;
    const AVFilterLink *inlink =
        ctx->inputs[dm->ppsrc ? INPUT_CLEANSRC : INPUT_MAIN];
    AVRational fps = inlink->frame_rate;
    if (!fps.num || !fps.den) {
        av_log(ctx, AV_LOG_ERROR, "The input needs a constant frame rate; "
               "current rate of %d/%d is invalid\n", fps.num, fps.den);
        return AVERROR(EINVAL);
    }
    fps = av_mul_q(fps, (AVRational){dm->cycle - 1, dm->cycle});
    av_log(ctx, AV_LOG_VERBOSE, "FPS: %d/%d -> %d/%d\n",
           inlink->frame_rate.num, inlink->frame_rate.den, fps.num, fps.den);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    outlink->time_base  = inlink->time_base;
    outlink->frame_rate = fps;
    outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    dm->ts_unit = av_q2d(av_inv_q(av_mul_q(fps, outlink->time_base)));
    return 0;
}
