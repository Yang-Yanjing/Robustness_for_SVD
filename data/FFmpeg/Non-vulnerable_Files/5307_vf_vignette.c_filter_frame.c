static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    unsigned x, y, direct = 0;
    AVFilterContext *ctx = inlink->dst;
    VignetteContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
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
    if (s->eval_mode == EVAL_MODE_FRAME)
        update_context(s, inlink, in);
    if (s->desc->flags & AV_PIX_FMT_FLAG_RGB) {
        uint8_t       *dst = out->data[0];
        const uint8_t *src = in ->data[0];
        const float *fmap = s->fmap;
        const int dst_linesize = out->linesize[0];
        const int src_linesize = in ->linesize[0];
        const int fmap_linesize = s->fmap_linesize;
        for (y = 0; y < inlink->h; y++) {
            uint8_t       *dstp = dst;
            const uint8_t *srcp = src;
            for (x = 0; x < inlink->w; x++, dstp += 3, srcp += 3) {
                const float f = fmap[x];
                dstp[0] = av_clip_uint8(srcp[0] * f + get_dither_value(s));
                dstp[1] = av_clip_uint8(srcp[1] * f + get_dither_value(s));
                dstp[2] = av_clip_uint8(srcp[2] * f + get_dither_value(s));
            }
            dst += dst_linesize;
            src += src_linesize;
            fmap += fmap_linesize;
        }
    } else {
        int plane;
        for (plane = 0; plane < 4 && in->data[plane] && in->linesize[plane]; plane++) {
            uint8_t       *dst = out->data[plane];
            const uint8_t *src = in ->data[plane];
            const float *fmap = s->fmap;
            const int dst_linesize = out->linesize[plane];
            const int src_linesize = in ->linesize[plane];
            const int fmap_linesize = s->fmap_linesize;
            const int chroma = plane == 1 || plane == 2;
            const int hsub = chroma ? s->desc->log2_chroma_w : 0;
            const int vsub = chroma ? s->desc->log2_chroma_h : 0;
            const int w = FF_CEIL_RSHIFT(inlink->w, hsub);
            const int h = FF_CEIL_RSHIFT(inlink->h, vsub);
            for (y = 0; y < h; y++) {
                uint8_t *dstp = dst;
                const uint8_t *srcp = src;
                for (x = 0; x < w; x++) {
                    const double dv = get_dither_value(s);
                    if (chroma) *dstp++ = av_clip_uint8(fmap[x << hsub] * (*srcp++ - 127) + 127 + dv);
                    else        *dstp++ = av_clip_uint8(fmap[x        ] *  *srcp++              + dv);
                }
                dst += dst_linesize;
                src += src_linesize;
                fmap += fmap_linesize << vsub;
            }
        }
    }
    if (!direct)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
