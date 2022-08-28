static int mpeg_mux_end(AVFormatContext *ctx)
{
    StreamInfo *stream;
    int i;
    for (;;) {
        int ret = output_packet(ctx, 1);
        if (ret < 0)
            return ret;
        else if (ret == 0)
            break;
    }
    


    
    
    for (i = 0; i < ctx->nb_streams; i++) {
        stream = ctx->streams[i]->priv_data;
        av_assert0(av_fifo_size(stream->fifo) == 0);
        av_fifo_freep(&stream->fifo);
    }
    return 0;
}
