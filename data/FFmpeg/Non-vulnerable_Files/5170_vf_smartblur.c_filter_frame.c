static int filter_frame(AVFilterLink *inlink, AVFrame *inpic)
{
    SmartblurContext  *s  = inlink->dst->priv;
    AVFilterLink *outlink     = inlink->dst->outputs[0];
    AVFrame *outpic;
    int cw = FF_CEIL_RSHIFT(inlink->w, s->hsub);
    int ch = FF_CEIL_RSHIFT(inlink->h, s->vsub);
    outpic = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!outpic) {
        av_frame_free(&inpic);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outpic, inpic);
    blur(outpic->data[0], outpic->linesize[0],
         inpic->data[0],  inpic->linesize[0],
         inlink->w, inlink->h, s->luma.threshold,
         s->luma.filter_context);
    if (inpic->data[2]) {
        blur(outpic->data[1], outpic->linesize[1],
             inpic->data[1],  inpic->linesize[1],
             cw, ch, s->chroma.threshold,
             s->chroma.filter_context);
        blur(outpic->data[2], outpic->linesize[2],
             inpic->data[2],  inpic->linesize[2],
             cw, ch, s->chroma.threshold,
             s->chroma.filter_context);
    }
    av_frame_free(&inpic);
    return ff_filter_frame(outlink, outpic);
}
