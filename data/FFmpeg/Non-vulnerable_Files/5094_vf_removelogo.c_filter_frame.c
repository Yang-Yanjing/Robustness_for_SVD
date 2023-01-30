static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    RemovelogoContext *s = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *outpicref;
    int direct = 0;
    if (av_frame_is_writable(inpicref)) {
        direct = 1;
        outpicref = inpicref;
    } else {
        outpicref = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!outpicref) {
            av_frame_free(&inpicref);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(outpicref, inpicref);
    }
    blur_image(s->mask,
               inpicref ->data[0], inpicref ->linesize[0],
               outpicref->data[0], outpicref->linesize[0],
               s->full_mask_data, inlink->w,
               inlink->w, inlink->h, direct, &s->full_mask_bbox);
    blur_image(s->mask,
               inpicref ->data[1], inpicref ->linesize[1],
               outpicref->data[1], outpicref->linesize[1],
               s->half_mask_data, inlink->w/2,
               inlink->w/2, inlink->h/2, direct, &s->half_mask_bbox);
    blur_image(s->mask,
               inpicref ->data[2], inpicref ->linesize[2],
               outpicref->data[2], outpicref->linesize[2],
               s->half_mask_data, inlink->w/2,
               inlink->w/2, inlink->h/2, direct, &s->half_mask_bbox);
    if (!direct)
        av_frame_free(&inpicref);
    return ff_filter_frame(outlink, outpicref);
}
