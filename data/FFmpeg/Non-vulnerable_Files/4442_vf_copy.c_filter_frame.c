static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out = ff_get_video_buffer(outlink, in->width, in->height);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    av_frame_copy(out, in);
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
