static int areverse_request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ReverseContext *s = ctx->priv;
    int ret, p, i, j;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && s->nb_frames > 0) {
        AVFrame *out = s->frames[s->nb_frames - 1];
        out->pts     = s->pts[s->flush_idx++];
        for (p = 0; p < outlink->channels; p++) {
            switch (outlink->format) {
            case AV_SAMPLE_FMT_U8P: {
                uint8_t *dst = (uint8_t *)out->extended_data[p];
                for (i = 0, j = out->nb_samples - 1; i < j; i++, j--)
                    FFSWAP(uint8_t, dst[i], dst[j]);
            }
                break;
            case AV_SAMPLE_FMT_S16P: {
                int16_t *dst = (int16_t *)out->extended_data[p];
                for (i = 0, j = out->nb_samples - 1; i < j; i++, j--)
                    FFSWAP(int16_t, dst[i], dst[j]);
            }
                break;
            case AV_SAMPLE_FMT_S32P: {
                int32_t *dst = (int32_t *)out->extended_data[p];
                for (i = 0, j = out->nb_samples - 1; i < j; i++, j--)
                    FFSWAP(int32_t, dst[i], dst[j]);
            }
                break;
            case AV_SAMPLE_FMT_FLTP: {
                float *dst = (float *)out->extended_data[p];
                for (i = 0, j = out->nb_samples - 1; i < j; i++, j--)
                    FFSWAP(float, dst[i], dst[j]);
            }
                break;
            case AV_SAMPLE_FMT_DBLP: {
                double *dst = (double *)out->extended_data[p];
                for (i = 0, j = out->nb_samples - 1; i < j; i++, j--)
                    FFSWAP(double, dst[i], dst[j]);
            }
                break;
            }
        }
        ret = ff_filter_frame(outlink, out);
        s->nb_frames--;
    }
    return ret;
}
