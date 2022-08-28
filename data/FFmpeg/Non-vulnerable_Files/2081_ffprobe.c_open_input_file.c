static int open_input_file(InputFile *ifile, const char *filename)
{
    int err, i, orig_nb_streams;
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *t;
    AVDictionary **opts;
    int scan_all_pmts_set = 0;
    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        print_error(filename, AVERROR(ENOMEM));
        exit_program(1);
    }
    fmt_ctx->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
    if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
        av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
        scan_all_pmts_set = 1;
    }
    if ((err = avformat_open_input(&fmt_ctx, filename,
                                   iformat, &format_opts)) < 0) {
        print_error(filename, err);
        return err;
    }
    ifile->fmt_ctx = fmt_ctx;
    if (scan_all_pmts_set)
        av_dict_set(&format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
    if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
        return AVERROR_OPTION_NOT_FOUND;
    }
    
    opts = setup_find_stream_info_opts(fmt_ctx, codec_opts);
    orig_nb_streams = fmt_ctx->nb_streams;
    err = avformat_find_stream_info(fmt_ctx, opts);
    for (i = 0; i < orig_nb_streams; i++)
        av_dict_free(&opts[i]);
    av_freep(&opts);
    if (err < 0) {
        print_error(filename, err);
        return err;
    }
    av_dump_format(fmt_ctx, 0, filename, 0);
    ifile->streams = av_mallocz_array(fmt_ctx->nb_streams,
                                      sizeof(*ifile->streams));
    if (!ifile->streams)
        exit(1);
    ifile->nb_streams = fmt_ctx->nb_streams;
    
    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        InputStream *ist = &ifile->streams[i];
        AVStream *stream = fmt_ctx->streams[i];
        AVCodec *codec;
        ist->st = stream;
        if (stream->codecpar->codec_id == AV_CODEC_ID_PROBE) {
            av_log(NULL, AV_LOG_WARNING,
                   "Failed to probe codec for input stream %d\n",
                    stream->index);
            continue;
        }
        codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if (!codec) {
            av_log(NULL, AV_LOG_WARNING,
                    "Unsupported codec with id %d for input stream %d\n",
                    stream->codecpar->codec_id, stream->index);
            continue;
        }
        {
            AVDictionary *opts = filter_codec_opts(codec_opts, stream->codecpar->codec_id,
                                                   fmt_ctx, stream, codec);
            ist->dec_ctx = avcodec_alloc_context3(codec);
            if (!ist->dec_ctx)
                exit(1);
            err = avcodec_parameters_to_context(ist->dec_ctx, stream->codecpar);
            if (err < 0)
                exit(1);
            if (do_show_log) {
                
                
                
                av_dict_set(&codec_opts, "threads", "1", 0);
            }
            av_codec_set_pkt_timebase(ist->dec_ctx, stream->time_base);
            ist->dec_ctx->framerate = stream->avg_frame_rate;
            if (avcodec_open2(ist->dec_ctx, codec, &opts) < 0) {
                av_log(NULL, AV_LOG_WARNING, "Could not open codec for input stream %d\n",
                       stream->index);
                exit(1);
            }
            if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
                av_log(NULL, AV_LOG_ERROR, "Option %s for input stream %d not found\n",
                       t->key, stream->index);
                return AVERROR_OPTION_NOT_FOUND;
            }
        }
    }
    ifile->fmt_ctx = fmt_ctx;
    return 0;
}
