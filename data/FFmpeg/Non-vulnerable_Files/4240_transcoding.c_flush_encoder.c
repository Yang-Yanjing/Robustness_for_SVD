static int flush_encoder(unsigned int stream_index)
{
    int ret;
    int got_frame;
    if (!(ofmt_ctx->streams[stream_index]->codec->codec->capabilities &
                AV_CODEC_CAP_DELAY))
        return 0;
    while (1) {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}
