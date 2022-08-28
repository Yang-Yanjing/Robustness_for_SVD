static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext  *ctx = outlink->src;
    ATempoContext *atempo = ctx->priv;
    int ret;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF) {
        
        int n_max = atempo->ring;
        int n_out;
        int err = AVERROR(EAGAIN);
        while (err == AVERROR(EAGAIN)) {
            if (!atempo->dst_buffer) {
                atempo->dst_buffer = ff_get_audio_buffer(outlink, n_max);
                if (!atempo->dst_buffer)
                    return AVERROR(ENOMEM);
                atempo->dst = atempo->dst_buffer->data[0];
                atempo->dst_end = atempo->dst + n_max * atempo->stride;
            }
            err = yae_flush(atempo, &atempo->dst, atempo->dst_end);
            n_out = ((atempo->dst - atempo->dst_buffer->data[0]) /
                     atempo->stride);
            if (n_out) {
                ret = push_samples(atempo, outlink, n_out);
            }
        }
        av_frame_free(&atempo->dst_buffer);
        atempo->dst     = NULL;
        atempo->dst_end = NULL;
        return AVERROR_EOF;
    }
    return ret;
}
