static int request_frame(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    ASyncContext      *s = ctx->priv;
    int ret = 0;
    int nb_samples;
    s->got_output = 0;
    while (ret >= 0 && !s->got_output)
        ret = ff_request_frame(ctx->inputs[0]);
    
    if (ret == AVERROR_EOF) {
        if (s->first_pts != AV_NOPTS_VALUE)
            handle_trimming(ctx);
        if (nb_samples = get_delay(s)) {
            AVFrame *buf = ff_get_audio_buffer(link, nb_samples);
            if (!buf)
                return AVERROR(ENOMEM);
            ret = avresample_convert(s->avr, buf->extended_data,
                                     buf->linesize[0], nb_samples, NULL, 0, 0);
            if (ret <= 0) {
                av_frame_free(&buf);
                return (ret < 0) ? ret : AVERROR_EOF;
            }
            buf->pts = s->pts;
            return ff_filter_frame(link, buf);
        }
    }
    return ret;
}
