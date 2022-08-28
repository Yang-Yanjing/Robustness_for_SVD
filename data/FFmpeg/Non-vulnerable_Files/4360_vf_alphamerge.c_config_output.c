static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *mainlink = ctx->inputs[0];
    AVFilterLink *alphalink = ctx->inputs[1];
    if (mainlink->w != alphalink->w || mainlink->h != alphalink->h) {
        av_log(ctx, AV_LOG_ERROR,
               "Input frame sizes do not match (%dx%d vs %dx%d).\n",
               mainlink->w, mainlink->h,
               alphalink->w, alphalink->h);
        return AVERROR(EINVAL);
    }
    outlink->w = mainlink->w;
    outlink->h = mainlink->h;
    outlink->time_base = mainlink->time_base;
    outlink->sample_aspect_ratio = mainlink->sample_aspect_ratio;
    outlink->frame_rate = mainlink->frame_rate;
    return 0;
}
