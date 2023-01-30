static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ResampleContext   *s = ctx->priv;
    int ret = 0;
    s->got_output = 0;
    while (ret >= 0 && !s->got_output)
        ret = ff_request_frame(ctx->inputs[0]);
    
    if (ret == AVERROR_EOF && s->avr) {
        AVFrame *frame;
        int nb_samples = avresample_get_out_samples(s->avr, 0);
        if (!nb_samples)
            return ret;
        frame = ff_get_audio_buffer(outlink, nb_samples);
        if (!frame)
            return AVERROR(ENOMEM);
        ret = avresample_convert(s->avr, frame->extended_data,
                                 frame->linesize[0], nb_samples,
                                 NULL, 0, 0);
        if (ret <= 0) {
            av_frame_free(&frame);
            return (ret == 0) ? AVERROR_EOF : ret;
        }
        frame->nb_samples = ret;
        frame->pts = s->next_pts;
        return ff_filter_frame(outlink, frame);
    }
    return ret;
}
