static int acrossfade_request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AudioFadeContext *s = ctx->priv;
    int ret = 0;
    if (!s->cf0_eof) {
        AVFilterLink *cf0 = ctx->inputs[0];
        ret = ff_request_frame(cf0);
        if (ret < 0 && ret != AVERROR_EOF)
            return ret;
        if (ret == AVERROR_EOF) {
            s->cf0_eof = 1;
            ret = 0;
        }
    } else {
        AVFilterLink *cf1 = ctx->inputs[1];
        int nb_samples = av_audio_fifo_size(s->fifo[1]);
        ret = ff_request_frame(cf1);
        if (ret == AVERROR_EOF && nb_samples > 0) {
            AVFrame *out = ff_get_audio_buffer(outlink, nb_samples);
            if (!out)
                return AVERROR(ENOMEM);
            av_audio_fifo_read(s->fifo[1], (void **)out->extended_data, nb_samples);
            ret = ff_filter_frame(outlink, out);
        }
    }
    return ret;
}
