static int config_out_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = outlink->src->inputs[0];
    InterlaceContext *s = ctx->priv;
    if (inlink->h < 2) {
        av_log(ctx, AV_LOG_ERROR, "input video height is too small\n");
        return AVERROR_INVALIDDATA;
    }
    if (!s->lowpass)
        av_log(ctx, AV_LOG_WARNING, "Lowpass filter is disabled, "
               "the resulting video will be aliased rather than interlaced.\n");
    
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    outlink->time_base = inlink->time_base;
    outlink->frame_rate = inlink->frame_rate;
    
    outlink->time_base.num *= 2;
    outlink->frame_rate.den *= 2;
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    if (s->lowpass) {
        s->lowpass_line = lowpass_line_c;
        if (ARCH_X86)
            ff_interlace_init_x86(s);
    }
    av_log(ctx, AV_LOG_VERBOSE, "%s interlacing %s lowpass filter\n",
           s->scan == MODE_TFF ? "tff" : "bff", (s->lowpass) ? "with" : "without");
    return 0;
}
