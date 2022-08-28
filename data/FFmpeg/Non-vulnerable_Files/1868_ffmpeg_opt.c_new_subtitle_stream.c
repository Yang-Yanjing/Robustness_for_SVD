static OutputStream *new_subtitle_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
    AVStream *st;
    OutputStream *ost;
    AVCodecContext *subtitle_enc;
    ost = new_output_stream(o, oc, AVMEDIA_TYPE_SUBTITLE, source_index);
    st  = ost->st;
    subtitle_enc = ost->enc_ctx;
    subtitle_enc->codec_type = AVMEDIA_TYPE_SUBTITLE;
    MATCH_PER_STREAM_OPT(copy_initial_nonkeyframes, i, ost->copy_initial_nonkeyframes, oc, st);
    if (!ost->stream_copy) {
        char *frame_size = NULL;
        MATCH_PER_STREAM_OPT(frame_sizes, str, frame_size, oc, st);
        if (frame_size && av_parse_video_size(&subtitle_enc->width, &subtitle_enc->height, frame_size) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", frame_size);
            exit_program(1);
        }
    }
    return ost;
}
