static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    NContext *s = ctx->priv;
    AVFrame *out;
    int plane, y;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    for (plane = 0; plane < s->nb_planes; plane++) {
        const int threshold = s->threshold[plane];
        if (threshold) {
            const uint8_t *src = in->data[plane];
            uint8_t *dst = out->data[plane];
            int stride = in->linesize[plane];
            int height = s->planeheight[plane];
            int width  = s->planewidth[plane];
            uint8_t *p0 = s->buffer + 16;
            uint8_t *p1 = p0 + s->planewidth[0];
            uint8_t *p2 = p1 + s->planewidth[0];
            uint8_t *orig = p0, *end = p2;
            line_copy8(p0, src + stride, width, 1);
            line_copy8(p1, src, width, 1);
            for (y = 0; y < height; y++) {
                const uint8_t *coordinates[] = { p0 - 1, p0, p0 + 1,
                                                 p1 - 1,     p1 + 1,
                                                 p2 - 1, p2, p2 + 1};
                src += stride * (y < height - 1 ? 1 : -1);
                line_copy8(p2, src, width, 1);
                s->filter(dst, p1, width, threshold, coordinates, s->coordinates);
                p0 = p1;
                p1 = p2;
                p2 = (p2 == end) ? orig: p2 + s->planewidth[0];
                dst += out->linesize[plane];
            }
        } else {
            av_image_copy_plane(out->data[plane], out->linesize[plane],
                                in->data[plane], in->linesize[plane],
                                s->planewidth[plane], s->planeheight[plane]);
        }
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
