static int compute_crc_of_packets(AVFormatContext *fmt_ctx, int video_stream,
                                AVCodecContext *ctx, AVFrame *fr, uint64_t ts_start, uint64_t ts_end, int no_seeking)
{
    int number_of_written_bytes;
    int got_frame = 0;
    int result;
    int end_of_stream = 0;
    int byte_buffer_size;
    uint8_t *byte_buffer;
    int64_t crc;
    AVPacket pkt;
    byte_buffer_size = av_image_get_buffer_size(ctx->pix_fmt, ctx->width, ctx->height, 16);
    byte_buffer = av_malloc(byte_buffer_size);
    if (!byte_buffer) {
        av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
        return AVERROR(ENOMEM);
    }
    if (!no_seeking) {
        result = av_seek_frame(fmt_ctx, video_stream, ts_start, AVSEEK_FLAG_ANY);
        printf("Seeking to %"PRId64", computing crc for frames with pts < %"PRId64"\n", ts_start, ts_end);
        if (result < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error in seeking\n");
            return result;
        }
        avcodec_flush_buffers(ctx);
    }
    av_init_packet(&pkt);
    do {
        if (!end_of_stream)
            if (av_read_frame(fmt_ctx, &pkt) < 0)
                end_of_stream = 1;
        if (end_of_stream) {
            pkt.data = NULL;
            pkt.size = 0;
        }
        if (pkt.stream_index == video_stream || end_of_stream) {
            got_frame = 0;
            if ((pkt.pts == AV_NOPTS_VALUE) && (!end_of_stream)) {
                av_log(NULL, AV_LOG_ERROR, "Error: frames doesn't have pts values\n");
                return -1;
            }
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
                if ((fr->pkt_pts > ts_end) && (!no_seeking))
                    break;
                crc = av_adler32_update(0, (const uint8_t*)byte_buffer, number_of_written_bytes);
                printf("%10"PRId64", 0x%08lx\n", fr->pkt_pts, crc);
                if (no_seeking) {
                    if (add_crc_to_array(crc, fr->pkt_pts) < 0)
                        return -1;
                }
                else {
                    if (compare_crc_in_array(crc, fr->pkt_pts) < 0)
                        return -1;
                }
            }
        }
        av_free_packet(&pkt);
        av_init_packet(&pkt);
    } while ((!end_of_stream || got_frame) && (no_seeking || (fr->pkt_pts + av_frame_get_pkt_duration(fr) <= ts_end)));
    av_free_packet(&pkt);
    av_freep(&byte_buffer);
    return 0;
}
