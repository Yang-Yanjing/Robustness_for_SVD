static int ifilter_send_frame(InputFilter *ifilter, AVFrame *frame)
{
    FilterGraph *fg = ifilter->graph;
    int need_reinit, ret, i;
    
    need_reinit = ifilter->format != frame->format;
    if (!!ifilter->hw_frames_ctx != !!frame->hw_frames_ctx ||
        (ifilter->hw_frames_ctx && ifilter->hw_frames_ctx->data != frame->hw_frames_ctx->data))
        need_reinit = 1;
    switch (ifilter->ist->st->codecpar->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        need_reinit |= ifilter->sample_rate    != frame->sample_rate ||
                       ifilter->channels       != frame->channels ||
                       ifilter->channel_layout != frame->channel_layout;
        break;
    case AVMEDIA_TYPE_VIDEO:
        need_reinit |= ifilter->width  != frame->width ||
                       ifilter->height != frame->height;
        break;
    }
    if (need_reinit) {
        ret = ifilter_parameters_from_frame(ifilter, frame);
        if (ret < 0)
            return ret;
    }
    
    if (need_reinit || !fg->graph) {
        for (i = 0; i < fg->nb_inputs; i++) {
            if (!ifilter_has_all_input_formats(fg)) {
                AVFrame *tmp = av_frame_clone(frame);
                if (!tmp)
                    return AVERROR(ENOMEM);
                av_frame_unref(frame);
                if (!av_fifo_space(ifilter->frame_queue)) {
                    ret = av_fifo_realloc2(ifilter->frame_queue, 2 * av_fifo_size(ifilter->frame_queue));
                    if (ret < 0) {
                        av_frame_free(&tmp);
                        return ret;
                    }
                }
                av_fifo_generic_write(ifilter->frame_queue, &tmp, sizeof(tmp), NULL);
                return 0;
            }
        }
        ret = reap_filters(1);
        if (ret < 0 && ret != AVERROR_EOF) {
            char errbuf[128];
            av_strerror(ret, errbuf, sizeof(errbuf));
            av_log(NULL, AV_LOG_ERROR, "Error while filtering: %s\n", errbuf);
            return ret;
        }
        ret = configure_filtergraph(fg);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error reinitializing filters!\n");
            return ret;
        }
    }
    ret = av_buffersrc_add_frame_flags(ifilter->filter, frame, AV_BUFFERSRC_FLAG_PUSH);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while filtering\n");
        return ret;
    }
    return 0;
}
