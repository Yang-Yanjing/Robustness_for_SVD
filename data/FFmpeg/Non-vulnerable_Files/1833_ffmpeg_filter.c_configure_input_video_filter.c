static int configure_input_video_filter(FilterGraph *fg, InputFilter *ifilter,
                                        AVFilterInOut *in)
{
    AVFilterContext *last_filter;
    const AVFilter *buffer_filt = avfilter_get_by_name("buffer");
    InputStream *ist = ifilter->ist;
    InputFile     *f = input_files[ist->file_index];
    AVRational tb = ist->framerate.num ? av_inv_q(ist->framerate) :
                                         ist->st->time_base;
    AVRational fr = ist->framerate;
    AVRational sar;
    AVBPrint args;
    char name[255];
    int ret, pad_idx = 0;
    int64_t tsoffset = 0;
    AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();
    if (!par)
        return AVERROR(ENOMEM);
    memset(par, 0, sizeof(*par));
    par->format = AV_PIX_FMT_NONE;
    if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        av_log(NULL, AV_LOG_ERROR, "Cannot connect video filter to audio input\n");
        ret = AVERROR(EINVAL);
        goto fail;
    }
    if (!fr.num)
        fr = av_guess_frame_rate(input_files[ist->file_index]->ctx, ist->st, NULL);
    if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
        ret = sub2video_prepare(ist, ifilter);
        if (ret < 0)
            goto fail;
    }
    sar = ifilter->sample_aspect_ratio;
    if(!sar.den)
        sar = (AVRational){0,1};
    av_bprint_init(&args, 0, 1);
    av_bprintf(&args,
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
             "pixel_aspect=%d/%d:sws_param=flags=%d",
             ifilter->width, ifilter->height, ifilter->format,
             tb.num, tb.den, sar.num, sar.den,
             SWS_BILINEAR + ((ist->dec_ctx->flags&AV_CODEC_FLAG_BITEXACT) ? SWS_BITEXACT:0));
    if (fr.num && fr.den)
        av_bprintf(&args, ":frame_rate=%d/%d", fr.num, fr.den);
    snprintf(name, sizeof(name), "graph %d input from stream %d:%d", fg->index,
             ist->file_index, ist->st->index);
    if ((ret = avfilter_graph_create_filter(&ifilter->filter, buffer_filt, name,
                                            args.str, NULL, fg->graph)) < 0)
        goto fail;
    par->hw_frames_ctx = ifilter->hw_frames_ctx;
    ret = av_buffersrc_parameters_set(ifilter->filter, par);
    if (ret < 0)
        goto fail;
    av_freep(&par);
    last_filter = ifilter->filter;
    if (ist->autorotate) {
        double theta = get_rotation(ist->st);
        if (fabs(theta - 90) < 1.0) {
            ret = insert_filter(&last_filter, &pad_idx, "transpose", "clock");
        } else if (fabs(theta - 180) < 1.0) {
            ret = insert_filter(&last_filter, &pad_idx, "hflip", NULL);
            if (ret < 0)
                return ret;
            ret = insert_filter(&last_filter, &pad_idx, "vflip", NULL);
        } else if (fabs(theta - 270) < 1.0) {
            ret = insert_filter(&last_filter, &pad_idx, "transpose", "cclock");
        } else if (fabs(theta) > 1.0) {
            char rotate_buf[64];
            snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
            ret = insert_filter(&last_filter, &pad_idx, "rotate", rotate_buf);
        }
        if (ret < 0)
            return ret;
    }
    if (do_deinterlace) {
        AVFilterContext *yadif;
        snprintf(name, sizeof(name), "deinterlace_in_%d_%d",
                 ist->file_index, ist->st->index);
        if ((ret = avfilter_graph_create_filter(&yadif,
                                                avfilter_get_by_name("yadif"),
                                                name, "", NULL,
                                                fg->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(last_filter, 0, yadif, 0)) < 0)
            return ret;
        last_filter = yadif;
    }
    snprintf(name, sizeof(name), "trim_in_%d_%d",
             ist->file_index, ist->st->index);
    if (copy_ts) {
        tsoffset = f->start_time == AV_NOPTS_VALUE ? 0 : f->start_time;
        if (!start_at_zero && f->ctx->start_time != AV_NOPTS_VALUE)
            tsoffset += f->ctx->start_time;
    }
    ret = insert_trim(((f->start_time == AV_NOPTS_VALUE) || !f->accurate_seek) ?
                      AV_NOPTS_VALUE : tsoffset, f->recording_time,
                      &last_filter, &pad_idx, name);
    if (ret < 0)
        return ret;
    if ((ret = avfilter_link(last_filter, 0, in->filter_ctx, in->pad_idx)) < 0)
        return ret;
    return 0;
fail:
    av_freep(&par);
    return ret;
}
