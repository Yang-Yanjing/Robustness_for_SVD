static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    ColorBalanceContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    const uint8_t roffset = s->rgba_map[R];
    const uint8_t goffset = s->rgba_map[G];
    const uint8_t boffset = s->rgba_map[B];
    const uint8_t aoffset = s->rgba_map[A];
    const int step = s->step;
    const uint8_t *srcrow = in->data[0];
    uint8_t *dstrow;
    AVFrame *out;
    int i, j;
    if (av_frame_is_writable(in)) {
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    dstrow = out->data[0];
    for (i = 0; i < outlink->h; i++) {
        const uint8_t *src = srcrow;
        uint8_t *dst = dstrow;
        for (j = 0; j < outlink->w * step; j += step) {
            dst[j + roffset] = s->lut[R][src[j + roffset]];
            dst[j + goffset] = s->lut[G][src[j + goffset]];
            dst[j + boffset] = s->lut[B][src[j + boffset]];
            if (in != out && step == 4)
                dst[j + aoffset] = src[j + aoffset];
        }
        srcrow += in->linesize[0];
        dstrow += out->linesize[0];
    }
    if (in != out)
        av_frame_free(&in);
    return ff_filter_frame(ctx->outputs[0], out);
}
