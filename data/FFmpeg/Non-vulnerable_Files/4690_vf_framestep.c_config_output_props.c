AVFILTER_DEFINE_CLASS(framestep);
static int config_output_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FrameStepContext *framestep = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    outlink->frame_rate =
        av_div_q(inlink->frame_rate, (AVRational){framestep->frame_step, 1});
    av_log(ctx, AV_LOG_VERBOSE, "step:%d frame_rate:%d/%d(%f) -> frame_rate:%d/%d(%f)\n",
           framestep->frame_step,
           inlink->frame_rate.num, inlink->frame_rate.den, av_q2d(inlink->frame_rate),
           outlink->frame_rate.num, outlink->frame_rate.den, av_q2d(outlink->frame_rate));
    return 0;
}
