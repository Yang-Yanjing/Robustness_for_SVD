int main(int argc, char **argv)
{
    int ret;
    AVPacket packet0, packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();
    int got_frame;
    if (!frame || !filt_frame) {
        perror("Could not allocate frame");
        exit(1);
    }
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file | %s\n", argv[0], player);
        exit(1);
    }
    av_register_all();
    avfilter_register_all();
    if ((ret = open_input_file(argv[1])) < 0)
        goto end;
    if ((ret = init_filters(filter_descr)) < 0)
        goto end;
    
    packet0.data = NULL;
    packet.data = NULL;
    while (1) {
        if (!packet0.data) {
            if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
                break;
            packet0 = packet;
        }
        if (packet.stream_index == audio_stream_index) {
            got_frame = 0;
            ret = avcodec_decode_audio4(dec_ctx, frame, &got_frame, &packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error decoding audio\n");
                continue;
            }
            packet.size -= ret;
            packet.data += ret;
            if (got_frame) {
                
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, 0) < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while feeding the audio filtergraph\n");
                    break;
                }
                
                while (1) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        goto end;
                    print_frame(filt_frame);
                    av_frame_unref(filt_frame);
                }
            }
            if (packet.size <= 0)
                av_free_packet(&packet0);
        } else {
            
            av_free_packet(&packet0);
        }
    }
end:
    avfilter_graph_free(&filter_graph);
    avcodec_close(dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        exit(1);
    }
    exit(0);
}
