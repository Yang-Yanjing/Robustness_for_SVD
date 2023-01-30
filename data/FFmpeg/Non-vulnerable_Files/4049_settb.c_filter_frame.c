static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    if (av_cmp_q(inlink->time_base, outlink->time_base)) {
        int64_t orig_pts = frame->pts;
        frame->pts = av_rescale_q(frame->pts, inlink->time_base, outlink->time_base);
        av_log(ctx, AV_LOG_DEBUG, "tb:%d/%d pts:%"PRId64" -> tb:%d/%d pts:%"PRId64"\n",
               inlink ->time_base.num, inlink ->time_base.den, orig_pts,
               outlink->time_base.num, outlink->time_base.den, frame->pts);
    }
    return ff_filter_frame(outlink, frame);
}
