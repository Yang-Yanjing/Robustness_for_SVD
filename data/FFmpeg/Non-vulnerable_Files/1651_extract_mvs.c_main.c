int main(int argc, char **argv)
{
    int ret = 0, got_frame;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(1);
    }
    src_filename = argv[1];
    av_register_all();
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;
    }
    av_dump_format(fmt_ctx, 0, src_filename, 0);
    if (!video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
    printf("framenum,source,blockw,blockh,srcx,srcy,dstx,dsty,flags\n");
    
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_free_packet(&orig_pkt);
    }
    
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);
end:
    avcodec_close(video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    return ret < 0;
}
