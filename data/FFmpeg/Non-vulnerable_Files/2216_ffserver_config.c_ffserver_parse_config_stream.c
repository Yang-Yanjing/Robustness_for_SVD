static int ffserver_parse_config_stream(FFServerConfig *config, const char *cmd,
                                        const char **p,
                                        FFServerStream **pstream)
{
    char arg[1024], arg2[1024];
    FFServerStream *stream;
    int val;
    av_assert0(pstream);
    stream = *pstream;
    if (!av_strcasecmp(cmd, "<Stream")) {
        char *q;
        FFServerStream *s;
        stream = av_mallocz(sizeof(FFServerStream));
        if (!stream)
            return AVERROR(ENOMEM);
        config->dummy_actx = avcodec_alloc_context3(NULL);
        config->dummy_vctx = avcodec_alloc_context3(NULL);
        if (!config->dummy_vctx || !config->dummy_actx) {
            av_free(stream);
            avcodec_free_context(&config->dummy_vctx);
            avcodec_free_context(&config->dummy_actx);
            return AVERROR(ENOMEM);
        }
        config->dummy_actx->codec_type = AVMEDIA_TYPE_AUDIO;
        config->dummy_vctx->codec_type = AVMEDIA_TYPE_VIDEO;
        ffserver_get_arg(stream->filename, sizeof(stream->filename), p);
        q = strrchr(stream->filename, '>');
        if (q)
            *q = '\0';
        for (s = config->first_stream; s; s = s->next) {
            if (!strcmp(stream->filename, s->filename))
                ERROR("Stream '%s' already registered\n", s->filename);
        }
        stream->fmt = ffserver_guess_format(NULL, stream->filename, NULL);
        if (stream->fmt) {
            config->guessed_audio_codec_id = stream->fmt->audio_codec;
            config->guessed_video_codec_id = stream->fmt->video_codec;
        } else {
            config->guessed_audio_codec_id = AV_CODEC_ID_NONE;
            config->guessed_video_codec_id = AV_CODEC_ID_NONE;
        }
        config->stream_use_defaults = config->use_defaults;
        *pstream = stream;
        return 0;
    }
    av_assert0(stream);
    if (!av_strcasecmp(cmd, "Feed")) {
        FFServerStream *sfeed;
        ffserver_get_arg(arg, sizeof(arg), p);
        sfeed = config->first_feed;
        while (sfeed) {
            if (!strcmp(sfeed->filename, arg))
                break;
            sfeed = sfeed->next_feed;
        }
        if (!sfeed)
            ERROR("Feed with name '%s' for stream '%s' is not defined\n", arg,
                    stream->filename);
        else
            stream->feed = sfeed;
    } else if (!av_strcasecmp(cmd, "Format")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (!strcmp(arg, "status")) {
            stream->stream_type = STREAM_TYPE_STATUS;
            stream->fmt = NULL;
        } else {
            stream->stream_type = STREAM_TYPE_LIVE;
            
            if (!strcmp(arg, "jpeg")) {
                strcpy(arg, "singlejpeg");
                stream->single_frame=1;
            }
            stream->fmt = ffserver_guess_format(arg, NULL, NULL);
            if (!stream->fmt)
                ERROR("Unknown Format: '%s'\n", arg);
        }
        if (stream->fmt) {
            config->guessed_audio_codec_id = stream->fmt->audio_codec;
            config->guessed_video_codec_id = stream->fmt->video_codec;
        }
    } else if (!av_strcasecmp(cmd, "InputFormat")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        stream->ifmt = av_find_input_format(arg);
        if (!stream->ifmt)
            ERROR("Unknown input format: '%s'\n", arg);
    } else if (!av_strcasecmp(cmd, "FaviconURL")) {
        if (stream->stream_type == STREAM_TYPE_STATUS)
            ffserver_get_arg(stream->feed_filename,
                    sizeof(stream->feed_filename), p);
        else
            ERROR("FaviconURL only permitted for status streams\n");
    } else if (!av_strcasecmp(cmd, "Author")    ||
               !av_strcasecmp(cmd, "Comment")   ||
               !av_strcasecmp(cmd, "Copyright") ||
               !av_strcasecmp(cmd, "Title")) {
        char key[32];
        int i;
        ffserver_get_arg(arg, sizeof(arg), p);
        for (i = 0; i < strlen(cmd); i++)
            key[i] = av_tolower(cmd[i]);
        key[i] = 0;
        WARNING("Deprecated '%s' option in configuration file. Use "
                "'Metadata %s VALUE' instead.\n", cmd, key);
        if (av_dict_set(&stream->metadata, key, arg, 0) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "Metadata")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_get_arg(arg2, sizeof(arg2), p);
        if (av_dict_set(&stream->metadata, arg, arg2, 0) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "Preroll")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        stream->prebuffer = atof(arg) * 1000;
    } else if (!av_strcasecmp(cmd, "StartSendOnKey")) {
        stream->send_on_key = 1;
    } else if (!av_strcasecmp(cmd, "AudioCodec")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_codec(config->dummy_actx, arg, config);
    } else if (!av_strcasecmp(cmd, "VideoCodec")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_codec(config->dummy_vctx, arg, config);
    } else if (!av_strcasecmp(cmd, "MaxTime")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        stream->max_time = atof(arg) * 1000;
    } else if (!av_strcasecmp(cmd, "AudioBitRate")) {
        float f;
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_float_param(&f, arg, 1000, -FLT_MAX, FLT_MAX, config,
                "Invalid %s: '%s'\n", cmd, arg);
        if (ffserver_save_avoption_int("b", (int64_t)lrintf(f),
                                       AV_OPT_FLAG_AUDIO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "AudioChannels")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("ac", arg, AV_OPT_FLAG_AUDIO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "AudioSampleRate")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("ar", arg, AV_OPT_FLAG_AUDIO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoBitRateRange")) {
        int minrate, maxrate;
        char *dash;
        ffserver_get_arg(arg, sizeof(arg), p);
        dash = strchr(arg, '-');
        if (dash) {
            *dash = '\0';
            dash++;
            if (ffserver_set_int_param(&minrate, arg,  1000, 0, INT_MAX, config, "Invalid %s: '%s'", cmd, arg) >= 0 &&
                ffserver_set_int_param(&maxrate, dash, 1000, 0, INT_MAX, config, "Invalid %s: '%s'", cmd, arg) >= 0) {
                if (ffserver_save_avoption_int("minrate", minrate, AV_OPT_FLAG_VIDEO_PARAM, config) < 0 ||
                    ffserver_save_avoption_int("maxrate", maxrate, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
                goto nomem;
            }
        } else
            ERROR("Incorrect format for VideoBitRateRange. It should be "
                  "<min>-<max>: '%s'.\n", arg);
    } else if (!av_strcasecmp(cmd, "Debug")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("debug", arg, AV_OPT_FLAG_AUDIO_PARAM, config) < 0 ||
            ffserver_save_avoption("debug", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "Strict")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("strict", arg, AV_OPT_FLAG_AUDIO_PARAM, config) < 0 ||
            ffserver_save_avoption("strict", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoBufferSize")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 8*1024, 0, INT_MAX, config,
                "Invalid %s: '%s'", cmd, arg);
        if (ffserver_save_avoption_int("bufsize", val, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoBitRateTolerance")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 1000, INT_MIN, INT_MAX, config,
                               "Invalid %s: '%s'", cmd, arg);
        if (ffserver_save_avoption_int("bt", val, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoBitRate")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 1000, INT_MIN, INT_MAX, config,
                               "Invalid %s: '%s'", cmd, arg);
        if (ffserver_save_avoption_int("b", val, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
           goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoSize")) {
        int ret, w, h;
        ffserver_get_arg(arg, sizeof(arg), p);
        ret = av_parse_video_size(&w, &h, arg);
        if (ret < 0)
            ERROR("Invalid video size '%s'\n", arg);
        else {
            if (w % 2 || h % 2)
                WARNING("Image size is not a multiple of 2\n");
            if (ffserver_save_avoption("video_size", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
                goto nomem;
        }
    } else if (!av_strcasecmp(cmd, "VideoFrameRate")) {
        ffserver_get_arg(&arg[2], sizeof(arg) - 2, p);
        arg[0] = '1'; arg[1] = '/';
        if (ffserver_save_avoption("time_base", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "PixelFormat")) {
        enum AVPixelFormat pix_fmt;
        ffserver_get_arg(arg, sizeof(arg), p);
        pix_fmt = av_get_pix_fmt(arg);
        if (pix_fmt == AV_PIX_FMT_NONE)
            ERROR("Unknown pixel format: '%s'\n", arg);
        else if (ffserver_save_avoption("pixel_format", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoGopSize")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("g", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoIntraOnly")) {
        if (ffserver_save_avoption("g", "1", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoHighQuality")) {
        if (ffserver_save_avoption("mbd", "+bits", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "Video4MotionVector")) {
        if (ffserver_save_avoption("mbd", "+bits",  AV_OPT_FLAG_VIDEO_PARAM, config) < 0 || 
            ffserver_save_avoption("flags", "+mv4", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "AVOptionVideo") ||
               !av_strcasecmp(cmd, "AVOptionAudio")) {
        int ret;
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_get_arg(arg2, sizeof(arg2), p);
        if (!av_strcasecmp(cmd, "AVOptionVideo"))
            ret = ffserver_save_avoption(arg, arg2, AV_OPT_FLAG_VIDEO_PARAM,
                                         config);
        else
            ret = ffserver_save_avoption(arg, arg2, AV_OPT_FLAG_AUDIO_PARAM,
                                         config);
        if (ret < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "AVPresetVideo") ||
               !av_strcasecmp(cmd, "AVPresetAudio")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (!av_strcasecmp(cmd, "AVPresetVideo"))
            ffserver_opt_preset(arg, AV_OPT_FLAG_VIDEO_PARAM, config);
        else
            ffserver_opt_preset(arg, AV_OPT_FLAG_AUDIO_PARAM, config);
    } else if (!av_strcasecmp(cmd, "VideoTag")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (strlen(arg) == 4 &&
            ffserver_save_avoption_int("codec_tag",
                                       MKTAG(arg[0], arg[1], arg[2], arg[3]),
                                       AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "BitExact")) {
        config->bitexact = 1;
        if (ffserver_save_avoption("flags", "+bitexact", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "DctFastint")) {
        if (ffserver_save_avoption("dct", "fastint", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "IdctSimple")) {
        if (ffserver_save_avoption("idct", "simple", AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "Qscale")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, INT_MIN, INT_MAX, config,
                               "Invalid Qscale: '%s'\n", arg);
        if (ffserver_save_avoption("flags", "+qscale", AV_OPT_FLAG_VIDEO_PARAM, config) < 0 ||
            ffserver_save_avoption_int("global_quality", FF_QP2LAMBDA * val,
                                       AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoQDiff")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("qdiff", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoQMax")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("qmax", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "VideoQMin")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("qmin", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "LumiMask")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("lumi_mask", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "DarkMask")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (ffserver_save_avoption("dark_mask", arg, AV_OPT_FLAG_VIDEO_PARAM, config) < 0)
            goto nomem;
    } else if (!av_strcasecmp(cmd, "NoVideo")) {
        config->no_video = 1;
    } else if (!av_strcasecmp(cmd, "NoAudio")) {
        config->no_audio = 1;
    } else if (!av_strcasecmp(cmd, "ACL")) {
        ffserver_parse_acl_row(stream, NULL, NULL, *p, config->filename,
                config->line_num);
    } else if (!av_strcasecmp(cmd, "DynamicACL")) {
        ffserver_get_arg(stream->dynamic_acl, sizeof(stream->dynamic_acl), p);
    } else if (!av_strcasecmp(cmd, "RTSPOption")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        av_freep(&stream->rtsp_option);
        stream->rtsp_option = av_strdup(arg);
    } else if (!av_strcasecmp(cmd, "MulticastAddress")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        if (resolve_host(&stream->multicast_ip, arg))
            ERROR("Invalid host/IP address: '%s'\n", arg);
        stream->is_multicast = 1;
        stream->loop = 1; 
    } else if (!av_strcasecmp(cmd, "MulticastPort")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, 1, 65535, config,
                "Invalid MulticastPort: '%s'\n", arg);
        stream->multicast_port = val;
    } else if (!av_strcasecmp(cmd, "MulticastTTL")) {
        ffserver_get_arg(arg, sizeof(arg), p);
        ffserver_set_int_param(&val, arg, 0, INT_MIN, INT_MAX, config,
                "Invalid MulticastTTL: '%s'\n", arg);
        stream->multicast_ttl = val;
    } else if (!av_strcasecmp(cmd, "NoLoop")) {
        stream->loop = 0;
    } else if (!av_strcasecmp(cmd, "</Stream>")) {
        config->stream_use_defaults &= 1;
        if (stream->feed && stream->fmt && strcmp(stream->fmt->name, "ffm")) {
            if (config->dummy_actx->codec_id == AV_CODEC_ID_NONE)
                config->dummy_actx->codec_id = config->guessed_audio_codec_id;
            if (!config->no_audio &&
                config->dummy_actx->codec_id != AV_CODEC_ID_NONE) {
                AVCodecContext *audio_enc = avcodec_alloc_context3(avcodec_find_encoder(config->dummy_actx->codec_id));
                add_codec(stream, audio_enc, config);
            }
            if (config->dummy_vctx->codec_id == AV_CODEC_ID_NONE)
                config->dummy_vctx->codec_id = config->guessed_video_codec_id;
            if (!config->no_video &&
                config->dummy_vctx->codec_id != AV_CODEC_ID_NONE) {
                AVCodecContext *video_enc = avcodec_alloc_context3(avcodec_find_encoder(config->dummy_vctx->codec_id));
                add_codec(stream, video_enc, config);
            }
        }
        av_dict_free(&config->video_opts);
        av_dict_free(&config->audio_opts);
        avcodec_free_context(&config->dummy_vctx);
        avcodec_free_context(&config->dummy_actx);
        config->no_video = 0;
        config->no_audio = 0;
        *pstream = NULL;
    } else if (!av_strcasecmp(cmd, "File") ||
               !av_strcasecmp(cmd, "ReadOnlyFile")) {
        ffserver_get_arg(stream->feed_filename, sizeof(stream->feed_filename),
                p);
    } else if (!av_strcasecmp(cmd, "UseDefaults")) {
        if (config->stream_use_defaults > 1)
            WARNING("Multiple UseDefaults/NoDefaults entries.\n");
        config->stream_use_defaults = 3;
    } else if (!av_strcasecmp(cmd, "NoDefaults")) {
        if (config->stream_use_defaults > 1)
            WARNING("Multiple UseDefaults/NoDefaults entries.\n");
        config->stream_use_defaults = 2;
    } else {
        ERROR("Invalid entry '%s' inside <Stream></Stream>\n", cmd);
    }
    return 0;
  nomem:
    av_log(NULL, AV_LOG_ERROR, "Out of memory. Aborting.\n");
    av_dict_free(&config->video_opts);
    av_dict_free(&config->audio_opts);
    avcodec_free_context(&config->dummy_vctx);
    avcodec_free_context(&config->dummy_actx);
    return AVERROR(ENOMEM);
}
