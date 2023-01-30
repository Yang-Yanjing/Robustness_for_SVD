static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    DCTdnoizContext *s = ctx->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    int direct, plane;
    AVFrame *out;
    if (av_frame_is_writable(in)) {
        direct = 1;
        out = in;
    } else {
        direct = 0;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    s->color_decorrelation(s->cbuf[0], s->p_linesize,
                           in->data[0], in->linesize[0],
                           s->pr_width, s->pr_height);
    for (plane = 0; plane < 3; plane++) {
        ThreadData td = {
            .src = s->cbuf[0][plane],
            .dst = s->cbuf[1][plane],
        };
        ctx->internal->execute(ctx, filter_slice, &td, NULL, s->nb_threads);
    }
    s->color_correlation(out->data[0], out->linesize[0],
                         s->cbuf[1], s->p_linesize,
                         s->pr_width, s->pr_height);
    if (!direct) {
        int y;
        uint8_t *dst = out->data[0];
        const uint8_t *src = in->data[0];
        const int dst_linesize = out->linesize[0];
        const int src_linesize = in->linesize[0];
        const int hpad = (inlink->w - s->pr_width) * 3;
        const int vpad = (inlink->h - s->pr_height);
        if (hpad) {
            uint8_t       *dstp = dst + s->pr_width * 3;
            const uint8_t *srcp = src + s->pr_width * 3;
            for (y = 0; y < s->pr_height; y++) {
                memcpy(dstp, srcp, hpad);
                dstp += dst_linesize;
                srcp += src_linesize;
            }
        }
        if (vpad) {
            uint8_t       *dstp = dst + s->pr_height * dst_linesize;
            const uint8_t *srcp = src + s->pr_height * src_linesize;
            for (y = 0; y < vpad; y++) {
                memcpy(dstp, srcp, inlink->w * 3);
                dstp += dst_linesize;
                srcp += src_linesize;
            }
        }
        av_frame_free(&in);
    }
    return ff_filter_frame(outlink, out);
}
