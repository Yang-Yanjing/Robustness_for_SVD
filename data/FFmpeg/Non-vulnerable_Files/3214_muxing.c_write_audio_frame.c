static int write_audio_frame(AVFormatContext *oc, OutputStream *ost)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; 
    AVFrame *frame;
    int ret;
    int got_packet;
    int dst_nb_samples;
    av_init_packet(&pkt);
    c = ost->st->codec;
    frame = get_audio_frame(ost);
    if (frame) {
        
            
            dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
                                            c->sample_rate, c->sample_rate, AV_ROUND_UP);
            av_assert0(dst_nb_samples == frame->nb_samples);
        



        ret = av_frame_make_writable(ost->frame);
        if (ret < 0)
            exit(1);
            
            ret = swr_convert(ost->swr_ctx,
                              ost->frame->data, dst_nb_samples,
                              (const uint8_t **)frame->data, frame->nb_samples);
            if (ret < 0) {
                fprintf(stderr, "Error while converting\n");
                exit(1);
            }
            frame = ost->frame;
        frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        ost->samples_count += dst_nb_samples;
    }
    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding audio frame: %s\n", av_err2str(ret));
        exit(1);
    }
    if (got_packet) {
        ret = write_frame(oc, &c->time_base, ost->st, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error while writing audio frame: %s\n",
                    av_err2str(ret));
            exit(1);
        }
    }
    return (frame || got_packet) ? 0 : 1;
}
