static void do_audio_out(OutputFile *of, OutputStream *ost,
                         AVFrame *frame)
{
    AVCodecContext *enc = ost->enc_ctx;
    AVPacket pkt;
    int ret;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    if (!check_recording_time(ost))
        return;
    if (frame->pts == AV_NOPTS_VALUE || audio_sync_method < 0)
        frame->pts = ost->sync_opts;
    ost->sync_opts = frame->pts + frame->nb_samples;
    ost->samples_encoded += frame->nb_samples;
    ost->frames_encoded++;
    av_assert0(pkt.size || !pkt.data);
    update_benchmark(NULL);
    if (debug_ts) {
        av_log(NULL, AV_LOG_INFO, "encoder <- type:audio "
               "frame_pts:%s frame_pts_time:%s time_base:%d/%d\n",
               av_ts2str(frame->pts), av_ts2timestr(frame->pts, &enc->time_base),
               enc->time_base.num, enc->time_base.den);
    }
    ret = avcodec_send_frame(enc, frame);
    if (ret < 0)
        goto error;
    while (1) {
        ret = avcodec_receive_packet(enc, &pkt);
        if (ret == AVERROR(EAGAIN))
            break;
        if (ret < 0)
            goto error;
        update_benchmark("encode_audio %d.%d", ost->file_index, ost->index);
        av_packet_rescale_ts(&pkt, enc->time_base, ost->mux_timebase);
        if (debug_ts) {
            av_log(NULL, AV_LOG_INFO, "encoder -> type:audio "
                   "pkt_pts:%s pkt_pts_time:%s pkt_dts:%s pkt_dts_time:%s\n",
                   av_ts2str(pkt.pts), av_ts2timestr(pkt.pts, &enc->time_base),
                   av_ts2str(pkt.dts), av_ts2timestr(pkt.dts, &enc->time_base));
        }
        output_packet(of, &pkt, ost);
    }
    return;
error:
    av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
    exit_program(1);
}
