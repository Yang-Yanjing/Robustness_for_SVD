static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    ColorChannelMixerContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    const uint8_t roffset = s->rgba_map[R];
    const uint8_t goffset = s->rgba_map[G];
    const uint8_t boffset = s->rgba_map[B];
    const uint8_t aoffset = s->rgba_map[A];
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
    switch (outlink->format) {
    case AV_PIX_FMT_BGR24:
    case AV_PIX_FMT_RGB24:
        for (i = 0; i < outlink->h; i++) {
            const uint8_t *src = srcrow;
            uint8_t *dst = dstrow;
            for (j = 0; j < outlink->w * 3; j += 3) {
                const uint8_t rin = src[j + roffset];
                const uint8_t gin = src[j + goffset];
                const uint8_t bin = src[j + boffset];
                dst[j + roffset] = av_clip_uint8(s->lut[R][R][rin] +
                                                 s->lut[R][G][gin] +
                                                 s->lut[R][B][bin]);
                dst[j + goffset] = av_clip_uint8(s->lut[G][R][rin] +
                                                 s->lut[G][G][gin] +
                                                 s->lut[G][B][bin]);
                dst[j + boffset] = av_clip_uint8(s->lut[B][R][rin] +
                                                 s->lut[B][G][gin] +
                                                 s->lut[B][B][bin]);
            }
            srcrow += in->linesize[0];
            dstrow += out->linesize[0];
        }
        break;
    case AV_PIX_FMT_0BGR:
    case AV_PIX_FMT_0RGB:
    case AV_PIX_FMT_BGR0:
    case AV_PIX_FMT_RGB0:
        for (i = 0; i < outlink->h; i++) {
            const uint8_t *src = srcrow;
            uint8_t *dst = dstrow;
            for (j = 0; j < outlink->w * 4; j += 4) {
                const uint8_t rin = src[j + roffset];
                const uint8_t gin = src[j + goffset];
                const uint8_t bin = src[j + boffset];
                dst[j + roffset] = av_clip_uint8(s->lut[R][R][rin] +
                                                 s->lut[R][G][gin] +
                                                 s->lut[R][B][bin]);
                dst[j + goffset] = av_clip_uint8(s->lut[G][R][rin] +
                                                 s->lut[G][G][gin] +
                                                 s->lut[G][B][bin]);
                dst[j + boffset] = av_clip_uint8(s->lut[B][R][rin] +
                                                 s->lut[B][G][gin] +
                                                 s->lut[B][B][bin]);
                if (in != out)
                    dst[j + aoffset] = 0;
            }
            srcrow += in->linesize[0];
            dstrow += out->linesize[0];
        }
        break;
    case AV_PIX_FMT_ABGR:
    case AV_PIX_FMT_ARGB:
    case AV_PIX_FMT_BGRA:
    case AV_PIX_FMT_RGBA:
        for (i = 0; i < outlink->h; i++) {
            const uint8_t *src = srcrow;
            uint8_t *dst = dstrow;
            for (j = 0; j < outlink->w * 4; j += 4) {
                const uint8_t rin = src[j + roffset];
                const uint8_t gin = src[j + goffset];
                const uint8_t bin = src[j + boffset];
                const uint8_t ain = src[j + aoffset];
                dst[j + roffset] = av_clip_uint8(s->lut[R][R][rin] +
                                                 s->lut[R][G][gin] +
                                                 s->lut[R][B][bin] +
                                                 s->lut[R][A][ain]);
                dst[j + goffset] = av_clip_uint8(s->lut[G][R][rin] +
                                                 s->lut[G][G][gin] +
                                                 s->lut[G][B][bin] +
                                                 s->lut[G][A][ain]);
                dst[j + boffset] = av_clip_uint8(s->lut[B][R][rin] +
                                                 s->lut[B][G][gin] +
                                                 s->lut[B][B][bin] +
                                                 s->lut[B][A][ain]);
                dst[j + aoffset] = av_clip_uint8(s->lut[A][R][rin] +
                                                 s->lut[A][G][gin] +
                                                 s->lut[A][B][bin] +
                                                 s->lut[A][A][ain]);
            }
            srcrow += in->linesize[0];
            dstrow += out->linesize[0];
        }
        break;
    case AV_PIX_FMT_BGR48:
    case AV_PIX_FMT_RGB48:
        for (i = 0; i < outlink->h; i++) {
            const uint16_t *src = (const uint16_t *)srcrow;
            uint16_t *dst = (uint16_t *)dstrow;
            for (j = 0; j < outlink->w * 3; j += 3) {
                const uint16_t rin = src[j + roffset];
                const uint16_t gin = src[j + goffset];
                const uint16_t bin = src[j + boffset];
                dst[j + roffset] = av_clip_uint16(s->lut[R][R][rin] +
                                                  s->lut[R][G][gin] +
                                                  s->lut[R][B][bin]);
                dst[j + goffset] = av_clip_uint16(s->lut[G][R][rin] +
                                                  s->lut[G][G][gin] +
                                                  s->lut[G][B][bin]);
                dst[j + boffset] = av_clip_uint16(s->lut[B][R][rin] +
                                                  s->lut[B][G][gin] +
                                                  s->lut[B][B][bin]);
            }
            srcrow += in->linesize[0];
            dstrow += out->linesize[0];
        }
        break;
    case AV_PIX_FMT_BGRA64:
    case AV_PIX_FMT_RGBA64:
        for (i = 0; i < outlink->h; i++) {
            const uint16_t *src = (const uint16_t *)srcrow;
            uint16_t *dst = (uint16_t *)dstrow;
            for (j = 0; j < outlink->w * 4; j += 4) {
                const uint16_t rin = src[j + roffset];
                const uint16_t gin = src[j + goffset];
                const uint16_t bin = src[j + boffset];
                const uint16_t ain = src[j + aoffset];
                dst[j + roffset] = av_clip_uint16(s->lut[R][R][rin] +
                                                  s->lut[R][G][gin] +
                                                  s->lut[R][B][bin] +
                                                  s->lut[R][A][ain]);
                dst[j + goffset] = av_clip_uint16(s->lut[G][R][rin] +
                                                  s->lut[G][G][gin] +
                                                  s->lut[G][B][bin] +
                                                  s->lut[G][A][ain]);
                dst[j + boffset] = av_clip_uint16(s->lut[B][R][rin] +
                                                  s->lut[B][G][gin] +
                                                  s->lut[B][B][bin] +
                                                  s->lut[B][A][ain]);
                dst[j + aoffset] = av_clip_uint16(s->lut[A][R][rin] +
                                                  s->lut[A][G][gin] +
                                                  s->lut[A][B][bin] +
                                                  s->lut[A][A][ain]);
            }
            srcrow += in->linesize[0];
            dstrow += out->linesize[0];
        }
    }
    if (in != out)
        av_frame_free(&in);
    return ff_filter_frame(ctx->outputs[0], out);
}
