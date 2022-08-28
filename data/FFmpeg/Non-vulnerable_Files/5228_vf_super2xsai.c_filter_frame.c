static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *outpicref = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!outpicref) {
        av_frame_free(&inpicref);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outpicref, inpicref);
    outpicref->width  = outlink->w;
    outpicref->height = outlink->h;
    super2xsai(inlink->dst, inpicref->data[0], inpicref->linesize[0],
               outpicref->data[0], outpicref->linesize[0],
               inlink->w, inlink->h);
    av_frame_free(&inpicref);
    return ff_filter_frame(outlink, outpicref);
}
