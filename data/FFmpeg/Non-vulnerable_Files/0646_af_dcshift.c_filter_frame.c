static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = ff_get_audio_buffer(inlink, in->nb_samples);
    DCShiftContext *s = ctx->priv;
    int i, j;
    double dcshift = s->dcshift;
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (s->limitergain > 0) {
        for (i = 0; i < inlink->channels; i++) {
            const int32_t *src = (int32_t *)in->extended_data[i];
            int32_t *dst = (int32_t *)out->extended_data[i];
            for (j = 0; j < in->nb_samples; j++) {
                double d;
                d = src[j];
                if (d > s->limiterthreshhold && dcshift > 0) {
                    d = (d - s->limiterthreshhold) * s->limitergain /
                             (INT32_MAX - s->limiterthreshhold) +
                             s->limiterthreshhold + dcshift;
                } else if (d < -s->limiterthreshhold && dcshift < 0) {
                    d = (d + s->limiterthreshhold) * s->limitergain /
                             (INT32_MAX - s->limiterthreshhold) -
                             s->limiterthreshhold + dcshift;
                } else {
                    d = dcshift * INT32_MAX + d;
                }
                dst[j] = av_clipl_int32(d);
            }
        }
    } else {
        for (i = 0; i < inlink->channels; i++) {
            const int32_t *src = (int32_t *)in->extended_data[i];
            int32_t *dst = (int32_t *)out->extended_data[i];
            for (j = 0; j < in->nb_samples; j++) {
                double d = dcshift * (INT32_MAX + 1.) + src[j];
                dst[j] = av_clipl_int32(d);
            }
        }
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
