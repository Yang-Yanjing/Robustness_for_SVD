static void add_codec(FFServerStream *stream, AVCodecContext *av,
                      FFServerConfig *config)
{
    LayeredAVStream *st;
    AVDictionary **opts, *recommended = NULL;
    char *enc_config;
    if(stream->nb_streams >= FF_ARRAY_ELEMS(stream->streams))
        return;
    opts = av->codec_type == AVMEDIA_TYPE_AUDIO ?
           &config->audio_opts : &config->video_opts;
    av_dict_copy(&recommended, *opts, 0);
    av_opt_set_dict2(av->priv_data, opts, AV_OPT_SEARCH_CHILDREN);
    av_opt_set_dict2(av, opts, AV_OPT_SEARCH_CHILDREN);
    if (av_dict_count(*opts))
        av_log(NULL, AV_LOG_WARNING,
               "Something is wrong, %d options are not set!\n",
               av_dict_count(*opts));
    if (!config->stream_use_defaults) {
        switch(av->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if (av->bit_rate == 0)
                report_config_error(config->filename, config->line_num,
                                    AV_LOG_ERROR, &config->errors,
                                    "audio bit rate is not set\n");
            if (av->sample_rate == 0)
                report_config_error(config->filename, config->line_num,
                                    AV_LOG_ERROR, &config->errors,
                                    "audio sample rate is not set\n");
            break;
        case AVMEDIA_TYPE_VIDEO:
            if (av->width == 0 || av->height == 0)
                report_config_error(config->filename, config->line_num,
                                    AV_LOG_ERROR, &config->errors,
                                    "video size is not set\n");
            break;
        default:
            av_assert0(0);
        }
        goto done;
    }
    
    
    switch(av->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        if (!av_dict_get(recommended, "b", NULL, 0)) {
            av->bit_rate = 64000;
            av_dict_set_int(&recommended, "b", av->bit_rate, 0);
            WARNING("Setting default value for audio bit rate = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->bit_rate);
        }
        if (!av_dict_get(recommended, "ar", NULL, 0)) {
            av->sample_rate = 22050;
            av_dict_set_int(&recommended, "ar", av->sample_rate, 0);
            WARNING("Setting default value for audio sample rate = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->sample_rate);
        }
        if (!av_dict_get(recommended, "ac", NULL, 0)) {
            av->channels = 1;
            av_dict_set_int(&recommended, "ac", av->channels, 0);
            WARNING("Setting default value for audio channel count = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->channels);
        }
        break;
    case AVMEDIA_TYPE_VIDEO:
        if (!av_dict_get(recommended, "b", NULL, 0)) {
            av->bit_rate = 64000;
            av_dict_set_int(&recommended, "b", av->bit_rate, 0);
            WARNING("Setting default value for video bit rate = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->bit_rate);
        }
        if (!av_dict_get(recommended, "time_base", NULL, 0)){
            av->time_base.den = 5;
            av->time_base.num = 1;
            av_dict_set(&recommended, "time_base", "1/5", 0);
            WARNING("Setting default value for video frame rate = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->time_base.den);
        }
        if (!av_dict_get(recommended, "video_size", NULL, 0)) {
            av->width = 160;
            av->height = 128;
            av_dict_set(&recommended, "video_size", "160x128", 0);
            WARNING("Setting default value for video size = %dx%d. "
                    "Use NoDefaults to disable it.\n",
                    av->width, av->height);
        }
        
        if (!av_dict_get(recommended, "bt", NULL, 0)) {
            av->bit_rate_tolerance = FFMAX(av->bit_rate / 4,
                      (int64_t)av->bit_rate*av->time_base.num/av->time_base.den);
            av_dict_set_int(&recommended, "bt", av->bit_rate_tolerance, 0);
            WARNING("Setting default value for video bit rate tolerance = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->bit_rate_tolerance);
        }
        if (!av_dict_get(recommended, "rc_eq", NULL, 0)) {
            av->rc_eq = av_strdup("tex^qComp");
            av_dict_set(&recommended, "rc_eq", "tex^qComp", 0);
            WARNING("Setting default value for video rate control equation = "
                    "%s. Use NoDefaults to disable it.\n",
                    av->rc_eq);
        }
        if (!av_dict_get(recommended, "maxrate", NULL, 0)) {
            av->rc_max_rate = av->bit_rate * 2;
            av_dict_set_int(&recommended, "maxrate", av->rc_max_rate, 0);
            WARNING("Setting default value for video max rate = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->rc_max_rate);
        }
        if (av->rc_max_rate && !av_dict_get(recommended, "bufsize", NULL, 0)) {
            av->rc_buffer_size = av->rc_max_rate;
            av_dict_set_int(&recommended, "bufsize", av->rc_buffer_size, 0);
            WARNING("Setting default value for video buffer size = %d. "
                    "Use NoDefaults to disable it.\n",
                    av->rc_buffer_size);
        }
        break;
    default:
        abort();
    }
done:
    st = av_mallocz(sizeof(*st));
    if (!st)
        return;
    av_dict_get_string(recommended, &enc_config, '=', ',');
    av_dict_free(&recommended);
    st->recommended_encoder_configuration = enc_config;
    st->codec = av;
    st->codecpar = avcodec_parameters_alloc();
    avcodec_parameters_from_context(st->codecpar, av);
    stream->streams[stream->nb_streams++] = st;
}
