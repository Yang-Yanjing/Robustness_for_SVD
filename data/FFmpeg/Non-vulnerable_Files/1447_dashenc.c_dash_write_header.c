static int dash_write_header(AVFormatContext *s)
{
    DASHContext *c = s->priv_data;
    int ret = 0, i;
    AVOutputFormat *oformat;
    char *ptr;
    char basename[1024];
    if (c->single_file_name)
        c->single_file = 1;
    if (c->single_file)
        c->use_template = 0;
    av_strlcpy(c->dirname, s->filename, sizeof(c->dirname));
    ptr = strrchr(c->dirname, '/');
    if (ptr) {
        av_strlcpy(basename, &ptr[1], sizeof(basename));
        ptr[1] = '\0';
    } else {
        c->dirname[0] = '\0';
        av_strlcpy(basename, s->filename, sizeof(basename));
    }
    ptr = strrchr(basename, '.');
    if (ptr)
        *ptr = '\0';
    oformat = av_guess_format("mp4", NULL, NULL);
    if (!oformat) {
        ret = AVERROR_MUXER_NOT_FOUND;
        goto fail;
    }
    c->streams = av_mallocz(sizeof(*c->streams) * s->nb_streams);
    if (!c->streams) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    for (i = 0; i < s->nb_streams; i++) {
        OutputStream *os = &c->streams[i];
        AVFormatContext *ctx;
        AVStream *st;
        AVDictionary *opts = NULL;
        char filename[1024];
        os->bit_rate = s->streams[i]->codec->bit_rate ?
                       s->streams[i]->codec->bit_rate :
                       s->streams[i]->codec->rc_max_rate;
        if (os->bit_rate) {
            snprintf(os->bandwidth_str, sizeof(os->bandwidth_str),
                     " bandwidth=\"%d\"", os->bit_rate);
        } else {
            int level = s->strict_std_compliance >= FF_COMPLIANCE_STRICT ?
                        AV_LOG_ERROR : AV_LOG_WARNING;
            av_log(s, level, "No bit rate set for stream %d\n", i);
            if (s->strict_std_compliance >= FF_COMPLIANCE_STRICT) {
                ret = AVERROR(EINVAL);
                goto fail;
            }
        }
        ctx = avformat_alloc_context();
        if (!ctx) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        os->ctx = ctx;
        ctx->oformat = oformat;
        ctx->interrupt_callback = s->interrupt_callback;
        if (!(st = avformat_new_stream(ctx, NULL))) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        avcodec_copy_context(st->codec, s->streams[i]->codec);
        st->sample_aspect_ratio = s->streams[i]->sample_aspect_ratio;
        st->time_base = s->streams[i]->time_base;
        ctx->avoid_negative_ts = s->avoid_negative_ts;
        ctx->pb = avio_alloc_context(os->iobuf, sizeof(os->iobuf), AVIO_FLAG_WRITE, os, NULL, dash_write, NULL);
        if (!ctx->pb) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if (c->single_file) {
            if (c->single_file_name)
                dash_fill_tmpl_params(os->initfile, sizeof(os->initfile), c->single_file_name, i, 0, os->bit_rate, 0);
            else
                snprintf(os->initfile, sizeof(os->initfile), "%s-stream%d.m4s", basename, i);
        } else {
            dash_fill_tmpl_params(os->initfile, sizeof(os->initfile), c->init_seg_name, i, 0, os->bit_rate, 0);
        }
        snprintf(filename, sizeof(filename), "%s%s", c->dirname, os->initfile);
        ret = ffurl_open(&os->out, filename, AVIO_FLAG_WRITE, &s->interrupt_callback, NULL);
        if (ret < 0)
            goto fail;
        os->init_start_pos = 0;
        av_dict_set(&opts, "movflags", "frag_custom+dash+delay_moov", 0);
        if ((ret = avformat_write_header(ctx, &opts)) < 0) {
             goto fail;
        }
        os->ctx_inited = 1;
        avio_flush(ctx->pb);
        av_dict_free(&opts);
        av_log(s, AV_LOG_VERBOSE, "Representation %d init segment will be written to: %s\n", i, filename);
        s->streams[i]->time_base = st->time_base;
        
        
        
        s->avoid_negative_ts = ctx->avoid_negative_ts;
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            c->has_video = 1;
        else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO)
            c->has_audio = 1;
        set_codec_str(s, st->codec, os->codec_str, sizeof(os->codec_str));
        os->first_pts = AV_NOPTS_VALUE;
        os->max_pts = AV_NOPTS_VALUE;
        os->last_dts = AV_NOPTS_VALUE;
        os->segment_index = 1;
    }
    if (!c->has_video && c->min_seg_duration <= 0) {
        av_log(s, AV_LOG_WARNING, "no video stream and no min seg duration set\n");
        ret = AVERROR(EINVAL);
    }
    ret = write_manifest(s, 0);
    if (!ret)
        av_log(s, AV_LOG_VERBOSE, "Manifest written to: %s\n", s->filename);
fail:
    if (ret)
        dash_free(s);
    return ret;
}
