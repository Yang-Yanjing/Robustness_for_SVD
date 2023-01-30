static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    WaveformContext *s    = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    int i, j, k;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    out->pts = in->pts;
    for (k = 0; k < s->ncomp; k++) {
        const int is_chroma = (k == 1 || k == 2);
        const int dst_h = FF_CEIL_RSHIFT(outlink->h, (is_chroma ? s->desc->log2_chroma_h : 0));
        const int dst_w = FF_CEIL_RSHIFT(outlink->w, (is_chroma ? s->desc->log2_chroma_w : 0));
        if (s->bits <= 8) {
            for (i = 0; i < dst_h ; i++)
                memset(out->data[s->desc->comp[k].plane] +
                       i * out->linesize[s->desc->comp[k].plane],
                       s->bg_color[k], dst_w);
        } else {
            const int mult = s->size / 256;
            uint16_t *dst = (uint16_t *)out->data[s->desc->comp[k].plane];
            for (i = 0; i < dst_h ; i++) {
                for (j = 0; j < dst_w; j++)
                    dst[j] = s->bg_color[k] * mult;
                dst += out->linesize[s->desc->comp[k].plane] / 2;
            }
        }
    }
    for (k = 0, i = 0; k < s->ncomp; k++) {
        if ((1 << k) & s->pcomp) {
            const int offset = i++ * s->size * s->display;
            s->waveform(s, in, out, k, s->intensity, offset, s->mode);
        }
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
