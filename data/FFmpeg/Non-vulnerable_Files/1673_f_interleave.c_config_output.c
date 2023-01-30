static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink0 = ctx->inputs[0];
    int i;
    if (outlink->type == AVMEDIA_TYPE_VIDEO) {
        outlink->time_base           = AV_TIME_BASE_Q;
        outlink->w                   = inlink0->w;
        outlink->h                   = inlink0->h;
        outlink->sample_aspect_ratio = inlink0->sample_aspect_ratio;
        outlink->format              = inlink0->format;
        outlink->frame_rate = (AVRational) {1, 0};
        for (i = 1; i < ctx->nb_inputs; i++) {
            AVFilterLink *inlink = ctx->inputs[i];
            if (outlink->w                       != inlink->w                       ||
                outlink->h                       != inlink->h                       ||
                outlink->sample_aspect_ratio.num != inlink->sample_aspect_ratio.num ||
                outlink->sample_aspect_ratio.den != inlink->sample_aspect_ratio.den) {
                av_log(ctx, AV_LOG_ERROR, "Parameters for input link %s "
                       "(size %dx%d, SAR %d:%d) do not match the corresponding "
                       "output link parameters (%dx%d, SAR %d:%d)\n",
                       ctx->input_pads[i].name, inlink->w, inlink->h,
                       inlink->sample_aspect_ratio.num,
                       inlink->sample_aspect_ratio.den,
                       outlink->w, outlink->h,
                       outlink->sample_aspect_ratio.num,
                       outlink->sample_aspect_ratio.den);
                return AVERROR(EINVAL);
            }
        }
    }
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
