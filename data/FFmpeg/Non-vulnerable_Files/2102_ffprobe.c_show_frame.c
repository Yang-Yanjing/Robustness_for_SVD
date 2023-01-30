static void show_frame(WriterContext *w, AVFrame *frame, AVStream *stream,
                       AVFormatContext *fmt_ctx)
{
    AVBPrint pbuf;
    char val_str[128];
    const char *s;
    int i;
    av_bprint_init(&pbuf, 1, AV_BPRINT_SIZE_UNLIMITED);
    writer_print_section_header(w, SECTION_ID_FRAME);
    s = av_get_media_type_string(stream->codecpar->codec_type);
    if (s) print_str    ("media_type", s);
    else   print_str_opt("media_type", "unknown");
    print_int("stream_index",           stream->index);
    print_int("key_frame",              frame->key_frame);
    print_ts  ("pkt_pts",               frame->pts);
    print_time("pkt_pts_time",          frame->pts, &stream->time_base);
    print_ts  ("pkt_dts",               frame->pkt_dts);
    print_time("pkt_dts_time",          frame->pkt_dts, &stream->time_base);
    print_ts  ("best_effort_timestamp", av_frame_get_best_effort_timestamp(frame));
    print_time("best_effort_timestamp_time", av_frame_get_best_effort_timestamp(frame), &stream->time_base);
    print_duration_ts  ("pkt_duration",      av_frame_get_pkt_duration(frame));
    print_duration_time("pkt_duration_time", av_frame_get_pkt_duration(frame), &stream->time_base);
    if (av_frame_get_pkt_pos (frame) != -1) print_fmt    ("pkt_pos", "%"PRId64, av_frame_get_pkt_pos(frame));
    else                      print_str_opt("pkt_pos", "N/A");
    if (av_frame_get_pkt_size(frame) != -1) print_val    ("pkt_size", av_frame_get_pkt_size(frame), unit_byte_str);
    else                       print_str_opt("pkt_size", "N/A");
    switch (stream->codecpar->codec_type) {
        AVRational sar;
    case AVMEDIA_TYPE_VIDEO:
        print_int("width",                  frame->width);
        print_int("height",                 frame->height);
        s = av_get_pix_fmt_name(frame->format);
        if (s) print_str    ("pix_fmt", s);
        else   print_str_opt("pix_fmt", "unknown");
        sar = av_guess_sample_aspect_ratio(fmt_ctx, stream, frame);
        if (sar.num) {
            print_q("sample_aspect_ratio", sar, ':');
        } else {
            print_str_opt("sample_aspect_ratio", "N/A");
        }
        print_fmt("pict_type",              "%c", av_get_picture_type_char(frame->pict_type));
        print_int("coded_picture_number",   frame->coded_picture_number);
        print_int("display_picture_number", frame->display_picture_number);
        print_int("interlaced_frame",       frame->interlaced_frame);
        print_int("top_field_first",        frame->top_field_first);
        print_int("repeat_pict",            frame->repeat_pict);
        break;
    case AVMEDIA_TYPE_AUDIO:
        s = av_get_sample_fmt_name(frame->format);
        if (s) print_str    ("sample_fmt", s);
        else   print_str_opt("sample_fmt", "unknown");
        print_int("nb_samples",         frame->nb_samples);
        print_int("channels", av_frame_get_channels(frame));
        if (av_frame_get_channel_layout(frame)) {
            av_bprint_clear(&pbuf);
            av_bprint_channel_layout(&pbuf, av_frame_get_channels(frame),
                                     av_frame_get_channel_layout(frame));
            print_str    ("channel_layout", pbuf.str);
        } else
            print_str_opt("channel_layout", "unknown");
        break;
    }
    if (do_show_frame_tags)
        show_tags(w, av_frame_get_metadata(frame), SECTION_ID_FRAME_TAGS);
    if (do_show_log)
        show_log(w, SECTION_ID_FRAME_LOGS, SECTION_ID_FRAME_LOG, do_show_log);
    if (frame->nb_side_data) {
        writer_print_section_header(w, SECTION_ID_FRAME_SIDE_DATA_LIST);
        for (i = 0; i < frame->nb_side_data; i++) {
            AVFrameSideData *sd = frame->side_data[i];
            const char *name;
            writer_print_section_header(w, SECTION_ID_FRAME_SIDE_DATA);
            name = av_frame_side_data_name(sd->type);
            print_str("side_data_type", name ? name : "unknown");
            if (sd->type == AV_FRAME_DATA_DISPLAYMATRIX && sd->size >= 9*4) {
                writer_print_integers(w, "displaymatrix", sd->data, 9, " %11d", 3, 4, 1);
                print_int("rotation", av_display_rotation_get((int32_t *)sd->data));
            } else if (sd->type == AV_FRAME_DATA_GOP_TIMECODE && sd->size >= 8) {
                char tcbuf[AV_TIMECODE_STR_SIZE];
                av_timecode_make_mpeg_tc_string(tcbuf, *(int64_t *)(sd->data));
                print_str("timecode", tcbuf);
            }
            writer_print_section_footer(w);
        }
        writer_print_section_footer(w);
    }
    writer_print_section_footer(w);
    av_bprint_finalize(&pbuf, NULL);
    fflush(stdout);
}
