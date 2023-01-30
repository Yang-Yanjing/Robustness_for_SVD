static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    int ret;
    AVFrame *out;
    AVFilterContext *ctx = inlink->dst;
    const ShowPaletteContext *s= ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    ret = disp_palette(out, in, s->size);
    av_frame_free(&in);
    return ret < 0 ? ret : ff_filter_frame(outlink, out);
}
