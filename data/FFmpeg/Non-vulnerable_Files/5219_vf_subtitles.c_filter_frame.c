static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AssContext *ass = ctx->priv;
    int detect_change = 0;
    double time_ms = picref->pts * av_q2d(inlink->time_base) * 1000;
    ASS_Image *image = ass_render_frame(ass->renderer, ass->track,
                                        time_ms, &detect_change);
    if (detect_change)
        av_log(ctx, AV_LOG_DEBUG, "Change happened at time ms:%f\n", time_ms);
    overlay_ass_image(ass, picref, image);
    return ff_filter_frame(outlink, picref);
}
