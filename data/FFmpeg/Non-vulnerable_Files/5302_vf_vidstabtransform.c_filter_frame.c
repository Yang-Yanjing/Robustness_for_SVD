static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    TransformContext *tc = ctx->priv;
    VSTransformData* td = &(tc->td);
    AVFilterLink *outlink = inlink->dst->outputs[0];
    int direct = 0;
    AVFrame *out;
    VSFrame inframe;
    int plane;
    if (av_frame_is_writable(in)) {
        direct = 1;
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    for (plane = 0; plane < vsTransformGetSrcFrameInfo(td)->planes; plane++) {
        inframe.data[plane] = in->data[plane];
        inframe.linesize[plane] = in->linesize[plane];
    }
    if (direct) {
        vsTransformPrepare(td, &inframe, &inframe);
    } else { 
        VSFrame outframe;
        for (plane = 0; plane < vsTransformGetDestFrameInfo(td)->planes; plane++) {
            outframe.data[plane] = out->data[plane];
            outframe.linesize[plane] = out->linesize[plane];
        }
        vsTransformPrepare(td, &inframe, &outframe);
    }
    vsDoTransform(td, vsGetNextTransform(td, &tc->trans));
    vsTransformFinish(td);
    if (!direct)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
