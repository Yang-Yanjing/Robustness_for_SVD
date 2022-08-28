static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    VectorscopeContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    uint8_t **dst;
    int i, k;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    out->pts = in->pts;
    dst = out->data;
    for (k = 0; k < 4 && dst[k]; k++)
        for (i = 0; i < outlink->h ; i++)
            memset(dst[k] + i * out->linesize[k],
                   s->mode == COLOR && k == s->pd ? 0 : s->bg_color[k], outlink->w);
    vectorscope(s, in, out, s->pd);
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
