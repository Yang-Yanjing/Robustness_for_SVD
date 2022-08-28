static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    HQDN3DContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    int c, direct = av_frame_is_writable(in) && !ctx->is_disabled;
    if (direct) {
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    for (c = 0; c < 3; c++) {
        denoise(s, in->data[c], out->data[c],
                s->line, &s->frame_prev[c],
                FF_CEIL_RSHIFT(in->width,  (!!c * s->hsub)),
                FF_CEIL_RSHIFT(in->height, (!!c * s->vsub)),
                in->linesize[c], out->linesize[c],
                s->coefs[c ? CHROMA_SPATIAL : LUMA_SPATIAL],
                s->coefs[c ? CHROMA_TMP     : LUMA_TMP]);
    }
    if (ctx->is_disabled) {
        av_frame_free(&out);
        return ff_filter_frame(outlink, in);
    }
    if (!direct)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
