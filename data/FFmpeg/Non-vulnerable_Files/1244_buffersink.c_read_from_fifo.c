static int read_from_fifo(AVFilterContext *ctx, AVFrame *frame,
                          int nb_samples)
{
    BufferSinkContext *s = ctx->priv;
    AVFilterLink   *link = ctx->inputs[0];
    AVFrame *tmp;
    if (!(tmp = ff_get_audio_buffer(link, nb_samples)))
        return AVERROR(ENOMEM);
    av_audio_fifo_read(s->audio_fifo, (void**)tmp->extended_data, nb_samples);
    tmp->pts = s->next_pts;
    if (s->next_pts != AV_NOPTS_VALUE)
        s->next_pts += av_rescale_q(nb_samples, (AVRational){1, link->sample_rate},
                                    link->time_base);
    av_frame_move_ref(frame, tmp);
    av_frame_free(&tmp);
    return 0;
}
