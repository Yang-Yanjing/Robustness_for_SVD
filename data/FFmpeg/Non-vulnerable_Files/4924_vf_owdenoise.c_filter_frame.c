static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    int direct = 0;
    AVFilterContext *ctx = inlink->dst;
    OWDenoiseContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    const int cw = FF_CEIL_RSHIFT(inlink->w, s->hsub);
    const int ch = FF_CEIL_RSHIFT(inlink->h, s->vsub);
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
    filter(s, out->data[0], out->linesize[0], in->data[0], in->linesize[0], inlink->w, inlink->h, s->luma_strength);
    filter(s, out->data[1], out->linesize[1], in->data[1], in->linesize[1], cw,        ch,        s->chroma_strength);
    filter(s, out->data[2], out->linesize[2], in->data[2], in->linesize[2], cw,        ch,        s->chroma_strength);
    if (!direct) {
        if (in->data[3])
            av_image_copy_plane(out->data[3], out->linesize[3],
                                in ->data[3], in ->linesize[3],
                                inlink->w, inlink->h);
        av_frame_free(&in);
    }
    return ff_filter_frame(outlink, out);
}
