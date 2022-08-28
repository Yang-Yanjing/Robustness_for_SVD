static int join_request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    JoinContext *s       = ctx->priv;
    AVFrame *frame;
    int linesize   = INT_MAX;
    int nb_samples = 0;
    int nb_buffers = 0;
    int i, j, ret;
    
    for (i = 0; i < ctx->nb_inputs; i++) {
        AVFilterLink *inlink = ctx->inputs[i];
        if (!s->input_frames[i] &&
            (ret = ff_request_frame(inlink)) < 0)
            return ret;
        
        if (i == 0) {
            nb_samples = s->input_frames[0]->nb_samples;
            for (j = 1; !i && j < ctx->nb_inputs; j++)
                ctx->inputs[j]->request_samples = nb_samples;
        }
    }
    
    frame = av_frame_alloc();
    if (!frame)
        return AVERROR(ENOMEM);
    if (s->nb_channels > FF_ARRAY_ELEMS(frame->data)) {
        frame->extended_data = av_mallocz_array(s->nb_channels,
                                          sizeof(*frame->extended_data));
        if (!frame->extended_data) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    
    for (i = 0; i < s->nb_channels; i++) {
        ChannelMap *ch = &s->channels[i];
        AVFrame *cur   = s->input_frames[ch->input];
        AVBufferRef *buf;
        frame->extended_data[i] = cur->extended_data[ch->in_channel_idx];
        linesize = FFMIN(linesize, cur->linesize[0]);
        

        buf = av_frame_get_plane_buffer(cur, ch->in_channel_idx);
        if (!buf) {
            ret = AVERROR(EINVAL);
            goto fail;
        }
        for (j = 0; j < nb_buffers; j++)
            if (s->buffers[j]->buffer == buf->buffer)
                break;
        if (j == i)
            s->buffers[nb_buffers++] = buf;
    }
    
    if (nb_buffers > FF_ARRAY_ELEMS(frame->buf)) {
        frame->nb_extended_buf = nb_buffers - FF_ARRAY_ELEMS(frame->buf);
        frame->extended_buf = av_mallocz_array(frame->nb_extended_buf,
                                               sizeof(*frame->extended_buf));
        if (!frame->extended_buf) {
            frame->nb_extended_buf = 0;
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    for (i = 0; i < FFMIN(FF_ARRAY_ELEMS(frame->buf), nb_buffers); i++) {
        frame->buf[i] = av_buffer_ref(s->buffers[i]);
        if (!frame->buf[i]) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    for (i = 0; i < frame->nb_extended_buf; i++) {
        frame->extended_buf[i] = av_buffer_ref(s->buffers[i +
                                               FF_ARRAY_ELEMS(frame->buf)]);
        if (!frame->extended_buf[i]) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    frame->nb_samples     = nb_samples;
    frame->channel_layout = outlink->channel_layout;
    av_frame_set_channels(frame, outlink->channels);
    frame->sample_rate    = outlink->sample_rate;
    frame->format         = outlink->format;
    frame->pts            = s->input_frames[0]->pts;
    frame->linesize[0]    = linesize;
    if (frame->data != frame->extended_data) {
        memcpy(frame->data, frame->extended_data, sizeof(*frame->data) *
               FFMIN(FF_ARRAY_ELEMS(frame->data), s->nb_channels));
    }
    ret = ff_filter_frame(outlink, frame);
    for (i = 0; i < ctx->nb_inputs; i++)
        av_frame_free(&s->input_frames[i]);
    return ret;
fail:
    av_frame_free(&frame);
    return ret;
}
