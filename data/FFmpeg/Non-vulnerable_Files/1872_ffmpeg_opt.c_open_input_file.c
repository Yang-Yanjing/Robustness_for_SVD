static int open_input_file(OptionsContext *o, const char *filename)
{
    InputFile *f;
    AVFormatContext *ic;
    AVInputFormat *file_iformat = NULL;
    int err, i, ret;
    int64_t timestamp;
    AVDictionary **opts;
    AVDictionary *unused_opts = NULL;
    AVDictionaryEntry *e = NULL;
    int orig_nb_streams;                     
    char *   video_codec_name = NULL;
    char *   audio_codec_name = NULL;
    char *subtitle_codec_name = NULL;
    char *    data_codec_name = NULL;
    int scan_all_pmts_set = 0;
    if (o->format) {
        if (!(file_iformat = av_find_input_format(o->format))) {
            av_log(NULL, AV_LOG_FATAL, "Unknown input format: '%s'\n", o->format);
            exit_program(1);
        }
    }
    if (!strcmp(filename, "-"))
        filename = "pipe:";
    stdin_interaction &= strncmp(filename, "pipe:", 5) &&
                         strcmp(filename, "/dev/stdin");
    
    ic = avformat_alloc_context();
    if (!ic) {
        print_error(filename, AVERROR(ENOMEM));
        exit_program(1);
    }
    ic->flags |= AVFMT_FLAG_KEEP_SIDE_DATA;
    if (o->nb_audio_sample_rate) {
        av_dict_set_int(&o->g->format_opts, "sample_rate", o->audio_sample_rate[o->nb_audio_sample_rate - 1].u.i, 0);
    }
    if (o->nb_audio_channels) {
        


        if (file_iformat && file_iformat->priv_class &&
            av_opt_find(&file_iformat->priv_class, "channels", NULL, 0,
                        AV_OPT_SEARCH_FAKE_OBJ)) {
            av_dict_set_int(&o->g->format_opts, "channels", o->audio_channels[o->nb_audio_channels - 1].u.i, 0);
        }
    }
    if (o->nb_frame_rates) {
        

        if (file_iformat && file_iformat->priv_class &&
            av_opt_find(&file_iformat->priv_class, "framerate", NULL, 0,
                        AV_OPT_SEARCH_FAKE_OBJ)) {
            av_dict_set(&o->g->format_opts, "framerate",
                        o->frame_rates[o->nb_frame_rates - 1].u.str, 0);
        }
    }
    if (o->nb_frame_sizes) {
        av_dict_set(&o->g->format_opts, "video_size", o->frame_sizes[o->nb_frame_sizes - 1].u.str, 0);
    }
    if (o->nb_frame_pix_fmts)
        av_dict_set(&o->g->format_opts, "pixel_format", o->frame_pix_fmts[o->nb_frame_pix_fmts - 1].u.str, 0);
    MATCH_PER_TYPE_OPT(codec_names, str,    video_codec_name, ic, "v");
    MATCH_PER_TYPE_OPT(codec_names, str,    audio_codec_name, ic, "a");
    MATCH_PER_TYPE_OPT(codec_names, str, subtitle_codec_name, ic, "s");
    MATCH_PER_TYPE_OPT(codec_names, str,     data_codec_name, ic, "d");
    ic->video_codec_id   = video_codec_name ?
        find_codec_or_die(video_codec_name   , AVMEDIA_TYPE_VIDEO   , 0)->id : AV_CODEC_ID_NONE;
    ic->audio_codec_id   = audio_codec_name ?
        find_codec_or_die(audio_codec_name   , AVMEDIA_TYPE_AUDIO   , 0)->id : AV_CODEC_ID_NONE;
    ic->subtitle_codec_id= subtitle_codec_name ?
        find_codec_or_die(subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 0)->id : AV_CODEC_ID_NONE;
    ic->data_codec_id    = data_codec_name ?
        find_codec_or_die(data_codec_name, AVMEDIA_TYPE_DATA, 0)->id : AV_CODEC_ID_NONE;
    if (video_codec_name)
        av_format_set_video_codec   (ic, find_codec_or_die(video_codec_name   , AVMEDIA_TYPE_VIDEO   , 0));
    if (audio_codec_name)
        av_format_set_audio_codec   (ic, find_codec_or_die(audio_codec_name   , AVMEDIA_TYPE_AUDIO   , 0));
    if (subtitle_codec_name)
        av_format_set_subtitle_codec(ic, find_codec_or_die(subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 0));
    if (data_codec_name)
        av_format_set_data_codec(ic, find_codec_or_die(data_codec_name, AVMEDIA_TYPE_DATA, 0));
    ic->flags |= AVFMT_FLAG_NONBLOCK;
    ic->interrupt_callback = int_cb;
    if (!av_dict_get(o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
        av_dict_set(&o->g->format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
        scan_all_pmts_set = 1;
    }
    
    err = avformat_open_input(&ic, filename, file_iformat, &o->g->format_opts);
    if (err < 0) {
        print_error(filename, err);
        if (err == AVERROR_PROTOCOL_NOT_FOUND)
            av_log(NULL, AV_LOG_ERROR, "Did you mean file:%s?\n", filename);
        exit_program(1);
    }
    if (scan_all_pmts_set)
        av_dict_set(&o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
    remove_avoptions(&o->g->format_opts, o->g->codec_opts);
    assert_avoptions(o->g->format_opts);
    
    for (i = 0; i < ic->nb_streams; i++)
        choose_decoder(o, ic, ic->streams[i]);
    
    opts = setup_find_stream_info_opts(ic, o->g->codec_opts);
    orig_nb_streams = ic->nb_streams;
    

    ret = avformat_find_stream_info(ic, opts);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "%s: could not find codec parameters\n", filename);
        if (ic->nb_streams == 0) {
            avformat_close_input(&ic);
            exit_program(1);
        }
    }
    if (o->start_time_eof != AV_NOPTS_VALUE) {
        if (ic->duration>0) {
            o->start_time = o->start_time_eof + ic->duration;
        } else
            av_log(NULL, AV_LOG_WARNING, "Cannot use -sseof, duration of %s not known\n", filename);
    }
    timestamp = (o->start_time == AV_NOPTS_VALUE) ? 0 : o->start_time;
    
    if (!o->seek_timestamp && ic->start_time != AV_NOPTS_VALUE)
        timestamp += ic->start_time;
    
    if (o->start_time != AV_NOPTS_VALUE) {
        int64_t seek_timestamp = timestamp;
        if (!(ic->iformat->flags & AVFMT_SEEK_TO_PTS)) {
            int dts_heuristic = 0;
            for (i=0; i<ic->nb_streams; i++) {
                const AVCodecParameters *par = ic->streams[i]->codecpar;
                if (par->video_delay)
                    dts_heuristic = 1;
            }
            if (dts_heuristic) {
                seek_timestamp -= 3*AV_TIME_BASE / 23;
            }
        }
        ret = avformat_seek_file(ic, -1, INT64_MIN, seek_timestamp, seek_timestamp, 0);
        if (ret < 0) {
            av_log(NULL, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
                   filename, (double)timestamp / AV_TIME_BASE);
        }
    }
    
    add_input_streams(o, ic);
    
    av_dump_format(ic, nb_input_files, filename, 0);
    GROW_ARRAY(input_files, nb_input_files);
    f = av_mallocz(sizeof(*f));
    if (!f)
        exit_program(1);
    input_files[nb_input_files - 1] = f;
    f->ctx        = ic;
    f->ist_index  = nb_input_streams - ic->nb_streams;
    f->start_time = o->start_time;
    f->recording_time = o->recording_time;
    f->input_ts_offset = o->input_ts_offset;
    f->ts_offset  = o->input_ts_offset - (copy_ts ? (start_at_zero && ic->start_time != AV_NOPTS_VALUE ? ic->start_time : 0) : timestamp);
    f->nb_streams = ic->nb_streams;
    f->rate_emu   = o->rate_emu;
    f->accurate_seek = o->accurate_seek;
    f->loop = o->loop;
    f->duration = 0;
    f->time_base = (AVRational){ 1, 1 };
#if HAVE_PTHREADS
    f->thread_queue_size = o->thread_queue_size > 0 ? o->thread_queue_size : 8;
#endif
    
    unused_opts = strip_specifiers(o->g->codec_opts);
    for (i = f->ist_index; i < nb_input_streams; i++) {
        e = NULL;
        while ((e = av_dict_get(input_streams[i]->decoder_opts, "", e,
                                AV_DICT_IGNORE_SUFFIX)))
            av_dict_set(&unused_opts, e->key, NULL, 0);
    }
    e = NULL;
    while ((e = av_dict_get(unused_opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
        const AVClass *class = avcodec_get_class();
        const AVOption *option = av_opt_find(&class, e->key, NULL, 0,
                                             AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
        const AVClass *fclass = avformat_get_class();
        const AVOption *foption = av_opt_find(&fclass, e->key, NULL, 0,
                                             AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
        if (!option || foption)
            continue;
        if (!(option->flags & AV_OPT_FLAG_DECODING_PARAM)) {
            av_log(NULL, AV_LOG_ERROR, "Codec AVOption %s (%s) specified for "
                   "input file #%d (%s) is not a decoding option.\n", e->key,
                   option->help ? option->help : "", nb_input_files - 1,
                   filename);
            exit_program(1);
        }
        av_log(NULL, AV_LOG_WARNING, "Codec AVOption %s (%s) specified for "
               "input file #%d (%s) has not been used for any stream. The most "
               "likely reason is either wrong type (e.g. a video option with "
               "no video streams) or that it is a private option of some decoder "
               "which was not actually used for any stream.\n", e->key,
               option->help ? option->help : "", nb_input_files - 1, filename);
    }
    av_dict_free(&unused_opts);
    for (i = 0; i < o->nb_dump_attachment; i++) {
        int j;
        for (j = 0; j < ic->nb_streams; j++) {
            AVStream *st = ic->streams[j];
            if (check_stream_specifier(ic, st, o->dump_attachment[i].specifier) == 1)
                dump_attachment(st, o->dump_attachment[i].u.str);
        }
    }
    for (i = 0; i < orig_nb_streams; i++)
        av_dict_free(&opts[i]);
    av_freep(&opts);
    input_stream_potentially_available = 1;
    return 0;
}
