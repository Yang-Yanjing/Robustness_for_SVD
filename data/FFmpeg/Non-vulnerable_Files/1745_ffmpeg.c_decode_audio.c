static int decode_audio(InputStream *ist, AVPacket *pkt, int *got_output,
                        int *decode_failed)
{
    AVFrame *decoded_frame;
    AVCodecContext *avctx = ist->dec_ctx;
    int ret, err = 0;
    AVRational decoded_frame_tb;
    if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    decoded_frame = ist->decoded_frame;
    update_benchmark(NULL);
    ret = decode(avctx, decoded_frame, got_output, pkt);
    update_benchmark("decode_audio %d.%d", ist->file_index, ist->st->index);
    if (ret < 0)
        *decode_failed = 1;
    if (ret >= 0 && avctx->sample_rate <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Sample rate %d invalid\n", avctx->sample_rate);
        ret = AVERROR_INVALIDDATA;
    }
    if (ret != AVERROR_EOF)
        check_decode_result(ist, got_output, ret);
    if (!*got_output || ret < 0)
        return ret;
    ist->samples_decoded += decoded_frame->nb_samples;
    ist->frames_decoded++;
#if 1
    

    ist->next_pts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
                     avctx->sample_rate;
    ist->next_dts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
                     avctx->sample_rate;
#endif
    if (decoded_frame->pts != AV_NOPTS_VALUE) {
        decoded_frame_tb   = ist->st->time_base;
    } else if (pkt && pkt->pts != AV_NOPTS_VALUE) {
        decoded_frame->pts = pkt->pts;
        decoded_frame_tb   = ist->st->time_base;
    }else {
        decoded_frame->pts = ist->dts;
        decoded_frame_tb   = AV_TIME_BASE_Q;
    }
    if (decoded_frame->pts != AV_NOPTS_VALUE)
        decoded_frame->pts = av_rescale_delta(decoded_frame_tb, decoded_frame->pts,
                                              (AVRational){1, avctx->sample_rate}, decoded_frame->nb_samples, &ist->filter_in_rescale_delta_last,
                                              (AVRational){1, avctx->sample_rate});
    ist->nb_samples = decoded_frame->nb_samples;
    err = send_frame_to_filters(ist, decoded_frame);
    av_frame_unref(ist->filter_frame);
    av_frame_unref(decoded_frame);
    return err < 0 ? err : ret;
}
