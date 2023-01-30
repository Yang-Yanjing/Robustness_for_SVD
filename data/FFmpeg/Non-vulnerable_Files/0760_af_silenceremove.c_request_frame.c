static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SilenceRemoveContext *s = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && (s->mode == SILENCE_COPY_FLUSH ||
                               s->mode == SILENCE_COPY)) {
        int nbs = s->stop_holdoff_end - s->stop_holdoff_offset;
        if (nbs) {
            AVFrame *frame;
            frame = ff_get_audio_buffer(outlink, nbs / outlink->channels);
            if (!frame)
                return AVERROR(ENOMEM);
            memcpy(frame->data[0], &s->stop_holdoff[s->stop_holdoff_offset],
                   nbs * sizeof(double));
            ret = ff_filter_frame(ctx->inputs[0], frame);
        }
        s->mode = SILENCE_STOP;
    }
    return ret;
}
