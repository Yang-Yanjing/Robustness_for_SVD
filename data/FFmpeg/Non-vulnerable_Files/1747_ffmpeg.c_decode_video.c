static int decode_video(InputStream *ist, AVPacket *pkt, int *got_output, int eof,
                        int *decode_failed)
{
    AVFrame *decoded_frame;
    int i, ret = 0, err = 0;
    int64_t best_effort_timestamp;
    int64_t dts = AV_NOPTS_VALUE;
    AVPacket avpkt;
    
    
    
    if (!eof && pkt && pkt->size == 0)
        return 0;
    if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
        return AVERROR(ENOMEM);
    decoded_frame = ist->decoded_frame;
    if (ist->dts != AV_NOPTS_VALUE)
        dts = av_rescale_q(ist->dts, AV_TIME_BASE_Q, ist->st->time_base);
    if (pkt) {
        avpkt = *pkt;
        avpkt.dts = dts; 
    }
    
    
    if (eof) {
        void *new = av_realloc_array(ist->dts_buffer, ist->nb_dts_buffer + 1, sizeof(ist->dts_buffer[0]));
        if (!new)
            return AVERROR(ENOMEM);
        ist->dts_buffer = new;
        ist->dts_buffer[ist->nb_dts_buffer++] = dts;
    }
    update_benchmark(NULL);
    ret = decode(ist->dec_ctx, decoded_frame, got_output, pkt ? &avpkt : NULL);
    update_benchmark("decode_video %d.%d", ist->file_index, ist->st->index);
    if (ret < 0)
        *decode_failed = 1;
    
    
    if (ist->st->codecpar->video_delay < ist->dec_ctx->has_b_frames) {
        if (ist->dec_ctx->codec_id == AV_CODEC_ID_H264) {
            ist->st->codecpar->video_delay = ist->dec_ctx->has_b_frames;
        } else
            av_log(ist->dec_ctx, AV_LOG_WARNING,
                   "video_delay is larger in decoder than demuxer %d > %d.\n"
                   "If you want to help, upload a sample "
                   "of this file to ftp://upload.ffmpeg.org/incoming/ "
                   "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n",
                   ist->dec_ctx->has_b_frames,
                   ist->st->codecpar->video_delay);
    }
    if (ret != AVERROR_EOF)
        check_decode_result(ist, got_output, ret);
    if (*got_output && ret >= 0) {
        if (ist->dec_ctx->width  != decoded_frame->width ||
            ist->dec_ctx->height != decoded_frame->height ||
            ist->dec_ctx->pix_fmt != decoded_frame->format) {
            av_log(NULL, AV_LOG_DEBUG, "Frame parameters mismatch context %d,%d,%d != %d,%d,%d\n",
                decoded_frame->width,
                decoded_frame->height,
                decoded_frame->format,
                ist->dec_ctx->width,
                ist->dec_ctx->height,
                ist->dec_ctx->pix_fmt);
        }
    }
    if (!*got_output || ret < 0)
        return ret;
    if(ist->top_field_first>=0)
        decoded_frame->top_field_first = ist->top_field_first;
    ist->frames_decoded++;
    if (ist->hwaccel_retrieve_data && decoded_frame->format == ist->hwaccel_pix_fmt) {
        err = ist->hwaccel_retrieve_data(ist->dec_ctx, decoded_frame);
        if (err < 0)
            goto fail;
    }
    ist->hwaccel_retrieved_pix_fmt = decoded_frame->format;
    best_effort_timestamp= av_frame_get_best_effort_timestamp(decoded_frame);
    if (ist->framerate.num)
        best_effort_timestamp = ist->cfr_next_pts++;
    if (eof && best_effort_timestamp == AV_NOPTS_VALUE && ist->nb_dts_buffer > 0) {
        best_effort_timestamp = ist->dts_buffer[0];
        for (i = 0; i < ist->nb_dts_buffer - 1; i++)
            ist->dts_buffer[i] = ist->dts_buffer[i + 1];
        ist->nb_dts_buffer--;
    }
    if(best_effort_timestamp != AV_NOPTS_VALUE) {
        int64_t ts = av_rescale_q(decoded_frame->pts = best_effort_timestamp, ist->st->time_base, AV_TIME_BASE_Q);
        if (ts != AV_NOPTS_VALUE)
            ist->next_pts = ist->pts = ts;
    }
    if (debug_ts) {
        av_log(NULL, AV_LOG_INFO, "decoder -> ist_index:%d type:video "
               "frame_pts:%s frame_pts_time:%s best_effort_ts:%"PRId64" best_effort_ts_time:%s keyframe:%d frame_type:%d time_base:%d/%d\n",
               ist->st->index, av_ts2str(decoded_frame->pts),
               av_ts2timestr(decoded_frame->pts, &ist->st->time_base),
               best_effort_timestamp,
               av_ts2timestr(best_effort_timestamp, &ist->st->time_base),
               decoded_frame->key_frame, decoded_frame->pict_type,
               ist->st->time_base.num, ist->st->time_base.den);
    }
    if (ist->st->sample_aspect_ratio.num)
        decoded_frame->sample_aspect_ratio = ist->st->sample_aspect_ratio;
    err = send_frame_to_filters(ist, decoded_frame);
fail:
    av_frame_unref(ist->filter_frame);
    av_frame_unref(decoded_frame);
    return err < 0 ? err : ret;
}
