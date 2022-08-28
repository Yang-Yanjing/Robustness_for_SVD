static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    StabData *sd = ctx->priv;
    VSMotionDetect *md = &(sd->md);
    LocalMotions localmotions;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    VSFrame frame;
    int plane;
    if (sd->conf.show > 0 && !av_frame_is_writable(in))
        av_frame_make_writable(in);
    for (plane = 0; plane < md->fi.planes; plane++) {
        frame.data[plane] = in->data[plane];
        frame.linesize[plane] = in->linesize[plane];
    }
    if (vsMotionDetection(md, &localmotions, &frame) != VS_OK) {
        av_log(ctx, AV_LOG_ERROR, "motion detection failed");
        return AVERROR(AVERROR_EXTERNAL);
    } else {
        if (vsWriteToFile(md, sd->f, &localmotions) != VS_OK) {
            int ret = AVERROR(errno);
            av_log(ctx, AV_LOG_ERROR, "cannot write to transform file");
            return ret;
        }
        vs_vector_del(&localmotions);
    }
    return ff_filter_frame(outlink, in);
}
