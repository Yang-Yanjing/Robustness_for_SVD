static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    PhaseContext *s = ctx->priv;
    enum PhaseMode mode;
    int plane, top, y;
    AVFrame *out;
    if (ctx->is_disabled) {
        av_frame_free(&s->frame);
        


        s->frame = av_frame_clone(in);
        return ff_filter_frame(outlink, in);
    }
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (!s->frame) {
        s->frame = in;
        mode = PROGRESSIVE;
    } else {
        mode = analyze_plane(ctx, s->mode, s->frame, in);
    }
    for (plane = 0; plane < s->nb_planes; plane++) {
        const uint8_t *buf = s->frame->data[plane];
        const uint8_t *from = in->data[plane];
        uint8_t *to = out->data[plane];
        for (y = 0, top = 1; y < s->planeheight[plane]; y++, top ^= 1) {
            memcpy(to, mode == (top ? BOTTOM_FIRST : TOP_FIRST) ? buf : from, s->linesize[plane]);
            buf += s->frame->linesize[plane];
            from += in->linesize[plane];
            to += out->linesize[plane];
        }
    }
    if (in != s->frame)
        av_frame_free(&s->frame);
    s->frame = in;
    return ff_filter_frame(outlink, out);
}
