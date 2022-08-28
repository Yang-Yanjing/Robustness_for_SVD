static int return_audio_frame(AVFilterContext *ctx)
{
    AVFilterLink *link = ctx->outputs[0];
    FifoContext *s = ctx->priv;
    AVFrame *head = s->root.next ? s->root.next->frame : NULL;
    AVFrame *out;
    int ret;
    
    if (!head && !s->out)
        return AVERROR_EOF;
    if (!s->out &&
        head->nb_samples >= link->request_samples &&
        calc_ptr_alignment(head) >= 32) {
        if (head->nb_samples == link->request_samples) {
            out = head;
            queue_pop(s);
        } else {
            out = av_frame_clone(head);
            if (!out)
                return AVERROR(ENOMEM);
            out->nb_samples = link->request_samples;
            buffer_offset(link, head, link->request_samples);
        }
    } else {
        int nb_channels = av_get_channel_layout_nb_channels(link->channel_layout);
        if (!s->out) {
            s->out = ff_get_audio_buffer(link, link->request_samples);
            if (!s->out)
                return AVERROR(ENOMEM);
            s->out->nb_samples = 0;
            s->out->pts                   = head->pts;
            s->allocated_samples          = link->request_samples;
        } else if (link->request_samples != s->allocated_samples) {
            av_log(ctx, AV_LOG_ERROR, "request_samples changed before the "
                   "buffer was returned.\n");
            return AVERROR(EINVAL);
        }
        while (s->out->nb_samples < s->allocated_samples) {
            int len;
            if (!s->root.next) {
                ret = ff_request_frame(ctx->inputs[0]);
                if (ret == AVERROR_EOF) {
                    av_samples_set_silence(s->out->extended_data,
                                           s->out->nb_samples,
                                           s->allocated_samples -
                                           s->out->nb_samples,
                                           nb_channels, link->format);
                    s->out->nb_samples = s->allocated_samples;
                    break;
                } else if (ret < 0)
                    return ret;
                av_assert0(s->root.next); 
            }
            head = s->root.next->frame;
            len = FFMIN(s->allocated_samples - s->out->nb_samples,
                        head->nb_samples);
            av_samples_copy(s->out->extended_data, head->extended_data,
                            s->out->nb_samples, 0, len, nb_channels,
                            link->format);
            s->out->nb_samples += len;
            if (len == head->nb_samples) {
                av_frame_free(&head);
                queue_pop(s);
            } else {
                buffer_offset(link, head, len);
            }
        }
        out = s->out;
        s->out = NULL;
    }
    return ff_filter_frame(link, out);
}
