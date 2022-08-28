static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ConcatContext *cat   = ctx->priv;
    unsigned out_no = FF_OUTLINK_IDX(outlink);
    unsigned in_no  = out_no, seg;
    AVFilterLink *inlink = ctx->inputs[in_no];
    
    outlink->time_base           = AV_TIME_BASE_Q;
    outlink->w                   = inlink->w;
    outlink->h                   = inlink->h;
    outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    outlink->format              = inlink->format;
    for (seg = 1; seg < cat->nb_segments; seg++) {
        inlink = ctx->inputs[in_no += ctx->nb_outputs];
        if (!outlink->sample_aspect_ratio.num)
            outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
        
        if (outlink->w                       != inlink->w                       ||
            outlink->h                       != inlink->h                       ||
            outlink->sample_aspect_ratio.num != inlink->sample_aspect_ratio.num &&
                                                inlink->sample_aspect_ratio.num ||
            outlink->sample_aspect_ratio.den != inlink->sample_aspect_ratio.den) {
            av_log(ctx, AV_LOG_ERROR, "Input link %s parameters "
                   "(size %dx%d, SAR %d:%d) do not match the corresponding "
                   "output link %s parameters (%dx%d, SAR %d:%d)\n",
                   ctx->input_pads[in_no].name, inlink->w, inlink->h,
                   inlink->sample_aspect_ratio.num,
                   inlink->sample_aspect_ratio.den,
                   ctx->input_pads[out_no].name, outlink->w, outlink->h,
                   outlink->sample_aspect_ratio.num,
                   outlink->sample_aspect_ratio.den);
            if (!cat->unsafe)
                return AVERROR(EINVAL);
        }
    }
    return 0;
}
