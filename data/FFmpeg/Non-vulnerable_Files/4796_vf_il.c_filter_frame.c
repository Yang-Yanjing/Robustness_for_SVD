static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    IlContext *s = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out;
    int comp;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&inpicref);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, inpicref);
    interleave(out->data[0], inpicref->data[0],
               s->linesize[0], inlink->h,
               out->linesize[0], inpicref->linesize[0],
               s->luma_mode, s->luma_swap);
    for (comp = 1; comp < (s->nb_planes - s->has_alpha); comp++) {
        interleave(out->data[comp], inpicref->data[comp],
                   s->linesize[comp], s->chroma_height,
                   out->linesize[comp], inpicref->linesize[comp],
                   s->chroma_mode, s->chroma_swap);
    }
    if (s->has_alpha) {
        comp = s->nb_planes - 1;
        interleave(out->data[comp], inpicref->data[comp],
                   s->linesize[comp], inlink->h,
                   out->linesize[comp], inpicref->linesize[comp],
                   s->alpha_mode, s->alpha_swap);
    }
    av_frame_free(&inpicref);
    return ff_filter_frame(outlink, out);
}
