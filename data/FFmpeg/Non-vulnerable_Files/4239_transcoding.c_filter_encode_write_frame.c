static int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
{
    int ret;
    AVFrame *filt_frame;
    av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    
    ret = av_buffersrc_add_frame_flags(filter_ctx[stream_index].buffersrc_ctx,
            frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }
    
    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame) {
            ret = AVERROR(ENOMEM);
            break;
        }
        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(filter_ctx[stream_index].buffersink_ctx,
                filt_frame);
        if (ret < 0) {
            



            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            av_frame_free(&filt_frame);
            break;
        }
        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encode_write_frame(filt_frame, stream_index, NULL);
        if (ret < 0)
            break;
    }
    return ret;
}
