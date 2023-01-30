static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    ShowFreqsContext *s = ctx->priv;
    AVFrame *fin = NULL;
    int ret = 0;
    av_audio_fifo_write(s->fifo, (void **)in->extended_data, in->nb_samples);
    while (av_audio_fifo_size(s->fifo) >= s->win_size) {
        fin = ff_get_audio_buffer(inlink, s->win_size);
        if (!fin) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        fin->pts = s->pts;
        s->pts += s->skip_samples;
        ret = av_audio_fifo_peek(s->fifo, (void **)fin->extended_data, s->win_size);
        if (ret < 0)
            goto fail;
        ret = plot_freqs(inlink, fin);
        av_frame_free(&fin);
        av_audio_fifo_drain(s->fifo, s->skip_samples);
        if (ret < 0)
            goto fail;
    }
fail:
    av_frame_free(&fin);
    av_frame_free(&in);
    return ret;
}
