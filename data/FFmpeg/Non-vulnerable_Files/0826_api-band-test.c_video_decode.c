static int video_decode(const char *input_filename)
{
    AVCodec *codec = NULL;
    AVCodecContext *origin_ctx = NULL, *ctx= NULL;
    uint8_t *byte_buffer = NULL;
    AVFrame *fr = NULL;
    AVPacket pkt;
    AVFormatContext *fmt_ctx = NULL;
    int number_of_written_bytes;
    int video_stream;
    int got_frame = 0;
    int byte_buffer_size;
    int result;
    int end_of_stream = 0;
    draw_horiz_band_called = 0;
    result = avformat_open_input(&fmt_ctx, input_filename, NULL, NULL);
    if (result < 0) {
        av_log(NULL, AV_LOG_ERROR, "Can't open file\n");
        return result;
    }
    result = avformat_find_stream_info(fmt_ctx, NULL);
    if (result < 0) {
        av_log(NULL, AV_LOG_ERROR, "Can't get stream info\n");
        return result;
    }
    video_stream = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream < 0) {
      av_log(NULL, AV_LOG_ERROR, "Can't find video stream in input file\n");
      return -1;
    }
    origin_ctx = fmt_ctx->streams[video_stream]->codec;
    codec = avcodec_find_decoder(origin_ctx->codec_id);
    if (!codec) {
        av_log(NULL, AV_LOG_ERROR, "Can't find decoder\n");
        return -1;
    }
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate decoder context\n");
        return AVERROR(ENOMEM);
    }
    result = avcodec_copy_context(ctx, origin_ctx);
    if (result) {
        av_log(NULL, AV_LOG_ERROR, "Can't copy decoder context\n");
        return result;
    }
    ctx->draw_horiz_band = draw_horiz_band;
    ctx->thread_count = 1;
    result = avcodec_open2(ctx, codec, NULL);
    if (result < 0) {
        av_log(ctx, AV_LOG_ERROR, "Can't open decoder\n");
        return result;
    }
    fr = av_frame_alloc();
    if (!fr) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate frame\n");
        return AVERROR(ENOMEM);
    }
    if (strcmp(codec->name, "flv") && strcmp(codec->name, "mpeg4") && strcmp(codec->name, "huffyuv")) {
        av_log(NULL, AV_LOG_ERROR, "Wrong codec\n");
        return -1;
    }
    byte_buffer_size = av_image_get_buffer_size(ctx->pix_fmt, ctx->width, ctx->height, 32);
    byte_buffer = av_malloc(byte_buffer_size);
    if (!byte_buffer) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
        return AVERROR(ENOMEM);
    }
    slice_byte_buffer = av_malloc(byte_buffer_size);
    if (!slice_byte_buffer) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
        return AVERROR(ENOMEM);
    }
    memset(slice_byte_buffer, 0, byte_buffer_size);
    slice_byte_buffer_size = byte_buffer_size;
    av_init_packet(&pkt);
    do {
        if (!end_of_stream) {
            if (av_read_frame(fmt_ctx, &pkt) < 0) {
                end_of_stream = 1;
            }
        }
        if (end_of_stream) {
            pkt.data = NULL;
            pkt.size = 0;
        }
        if (pkt.stream_index == video_stream || end_of_stream) {
            got_frame = 0;
            result = avcodec_decode_video2(ctx, fr, &got_frame, &pkt);
            if (result < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error decoding frame\n");
                return result;
            }
            if (got_frame) {
                number_of_written_bytes = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
                                        (const uint8_t* const *)fr->data, (const int*) fr->linesize,
                                        ctx->pix_fmt, ctx->width, ctx->height, 1);
                if (number_of_written_bytes < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Can't copy image to buffer\n");
                    return number_of_written_bytes;
                }
                if (draw_horiz_band_called == 0) {
                    av_log(NULL, AV_LOG_ERROR, "draw_horiz_band haven't been called!\n");
                    return -1;
                }
                if (av_adler32_update(0, (const uint8_t*)byte_buffer, number_of_written_bytes) !=
                    av_adler32_update(0, (const uint8_t*)slice_byte_buffer, number_of_written_bytes)) {
                    av_log(NULL, AV_LOG_ERROR, "Decoded frames with and without draw_horiz_band are not the same!\n");
                    return -1;
                }
            }
            av_free_packet(&pkt);
            av_init_packet(&pkt);
        }
    } while (!end_of_stream || got_frame);
    av_free_packet(&pkt);
    av_frame_free(&fr);
    avcodec_close(ctx);
    avformat_close_input(&fmt_ctx);
    avcodec_free_context(&ctx);
    av_freep(&byte_buffer);
    av_freep(&slice_byte_buffer);
    return 0;
}
