static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AudioPhaseMeterContext *s = ctx->priv;
    AVDictionary **metadata;
    const int rc = s->contrast[0];
    const int gc = s->contrast[1];
    const int bc = s->contrast[2];
    float fphase = 0;
    AVFrame *out;
    uint8_t *dst;
    int i;
    if (!s->out || s->out->width  != outlink->w ||
                   s->out->height != outlink->h) {
        av_frame_free(&s->out);
        s->out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!s->out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        out = s->out;
        for (i = 0; i < outlink->h; i++)
            memset(out->data[0] + i * out->linesize[0], 0, outlink->w * 4);
    } else {
        out = s->out;
        for (i = outlink->h - 1; i >= 10; i--)
            memmove(out->data[0] + (i  ) * out->linesize[0],
                    out->data[0] + (i-1) * out->linesize[0],
                    outlink->w * 4);
        for (i = 0; i < outlink->w; i++)
            AV_WL32(out->data[0] + i * 4, 0);
    }
    s->out->pts = in->pts;
    for (i = 0; i < in->nb_samples; i++) {
        const float *src = (float *)in->data[0] + i * 2;
        const float f = src[0] * src[1] / (src[0]*src[0] + src[1] * src[1]) * 2;
        const float phase = isnan(f) ? 1 : f;
        const int x = get_x(phase, s->w);
        dst = out->data[0] + x * 4;
        dst[0] = FFMIN(255, dst[0] + rc);
        dst[1] = FFMIN(255, dst[1] + gc);
        dst[2] = FFMIN(255, dst[2] + bc);
        dst[3] = 255;
        fphase += phase;
    }
    fphase /= in->nb_samples;
    if (s->draw_median_phase) {
        dst = out->data[0] + get_x(fphase, s->w) * 4;
        AV_WL32(dst, AV_RL32(s->mpc));
    }
    for (i = 1; i < 10 && i < outlink->h; i++)
        memcpy(out->data[0] + i * out->linesize[0], out->data[0], outlink->w * 4);
    metadata = avpriv_frame_get_metadatap(out);
    if (metadata) {
        uint8_t value[128];
        snprintf(value, sizeof(value), "%f", fphase);
        av_dict_set(metadata, "lavfi.aphasemeter.phase", value, 0);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, av_frame_clone(s->out));
}
