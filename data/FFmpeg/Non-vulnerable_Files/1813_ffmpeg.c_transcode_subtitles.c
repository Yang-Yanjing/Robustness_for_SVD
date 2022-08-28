static int transcode_subtitles(InputStream *ist, AVPacket *pkt, int *got_output,
                               int *decode_failed)
{
    AVSubtitle subtitle;
    int free_sub = 1;
    int i, ret = avcodec_decode_subtitle2(ist->dec_ctx,
                                          &subtitle, got_output, pkt);
    check_decode_result(NULL, got_output, ret);
    if (ret < 0 || !*got_output) {
        *decode_failed = 1;
        if (!pkt->size)
            sub2video_flush(ist);
        return ret;
    }
    if (ist->fix_sub_duration) {
        int end = 1;
        if (ist->prev_sub.got_output) {
            end = av_rescale(subtitle.pts - ist->prev_sub.subtitle.pts,
                             1000, AV_TIME_BASE);
            if (end < ist->prev_sub.subtitle.end_display_time) {
                av_log(ist->dec_ctx, AV_LOG_DEBUG,
                       "Subtitle duration reduced from %"PRId32" to %d%s\n",
                       ist->prev_sub.subtitle.end_display_time, end,
                       end <= 0 ? ", dropping it" : "");
                ist->prev_sub.subtitle.end_display_time = end;
            }
        }
        FFSWAP(int,        *got_output, ist->prev_sub.got_output);
        FFSWAP(int,        ret,         ist->prev_sub.ret);
        FFSWAP(AVSubtitle, subtitle,    ist->prev_sub.subtitle);
        if (end <= 0)
            goto out;
    }
    if (!*got_output)
        return ret;
    if (ist->sub2video.frame) {
        sub2video_update(ist, &subtitle);
    } else if (ist->nb_filters) {
        if (!ist->sub2video.sub_queue)
            ist->sub2video.sub_queue = av_fifo_alloc(8 * sizeof(AVSubtitle));
        if (!ist->sub2video.sub_queue)
            exit_program(1);
        if (!av_fifo_space(ist->sub2video.sub_queue)) {
            ret = av_fifo_realloc2(ist->sub2video.sub_queue, 2 * av_fifo_size(ist->sub2video.sub_queue));
            if (ret < 0)
                exit_program(1);
        }
        av_fifo_generic_write(ist->sub2video.sub_queue, &subtitle, sizeof(subtitle), NULL);
        free_sub = 0;
    }
    if (!subtitle.num_rects)
        goto out;
    ist->frames_decoded++;
    for (i = 0; i < nb_output_streams; i++) {
        OutputStream *ost = output_streams[i];
        if (!check_output_constraints(ist, ost) || !ost->encoding_needed
            || ost->enc->type != AVMEDIA_TYPE_SUBTITLE)
            continue;
        do_subtitle_out(output_files[ost->file_index], ost, &subtitle);
    }
out:
    if (free_sub)
        avsubtitle_free(&subtitle);
    return ret;
}
