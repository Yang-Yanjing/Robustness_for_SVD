AVFILTER_DEFINE_CLASS(subtitles);
static av_cold int init_subtitles(AVFilterContext *ctx)
{
    int j, ret, sid;
    int k = 0;
    AVDictionary *codec_opts = NULL;
    AVFormatContext *fmt = NULL;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    const AVCodecDescriptor *dec_desc;
    AVStream *st;
    AVPacket pkt;
    AssContext *ass = ctx->priv;
    
    ret = init(ctx);
    if (ret < 0)
        return ret;
    ass->track = ass_new_track(ass->library);
    if (!ass->track) {
        av_log(ctx, AV_LOG_ERROR, "Could not create a libass track\n");
        return AVERROR(EINVAL);
    }
    
    ret = avformat_open_input(&fmt, ass->filename, NULL, NULL);
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to open %s\n", ass->filename);
        goto end;
    }
    ret = avformat_find_stream_info(fmt, NULL);
    if (ret < 0)
        goto end;
    
    if (ass->stream_index < 0)
        ret = av_find_best_stream(fmt, AVMEDIA_TYPE_SUBTITLE, -1, -1, NULL, 0);
    else {
        ret = -1;
        if (ass->stream_index < fmt->nb_streams) {
            for (j = 0; j < fmt->nb_streams; j++) {
                if (fmt->streams[j]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE) {
                    if (ass->stream_index == k) {
                        ret = j;
                        break;
                    }
                    k++;
                }
            }
        }
    }
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to locate subtitle stream in %s\n",
               ass->filename);
        goto end;
    }
    sid = ret;
    st = fmt->streams[sid];
    
    for (j = 0; j < fmt->nb_streams; j++) {
        AVStream *st = fmt->streams[j];
        if (st->codec->codec_type == AVMEDIA_TYPE_ATTACHMENT &&
            attachment_is_font(st)) {
            const AVDictionaryEntry *tag = NULL;
            tag = av_dict_get(st->metadata, "filename", NULL,
                              AV_DICT_MATCH_CASE);
            if (tag) {
                av_log(ctx, AV_LOG_DEBUG, "Loading attached font: %s\n",
                       tag->value);
                ass_add_font(ass->library, tag->value,
                             st->codec->extradata,
                             st->codec->extradata_size);
            } else {
                av_log(ctx, AV_LOG_WARNING,
                       "Font attachment has no filename, ignored.\n");
            }
        }
    }
    
    ass_set_fonts(ass->renderer, NULL, NULL, 1, NULL, 1);
    
    dec_ctx = st->codec;
    dec = avcodec_find_decoder(dec_ctx->codec_id);
    if (!dec) {
        av_log(ctx, AV_LOG_ERROR, "Failed to find subtitle codec %s\n",
               avcodec_get_name(dec_ctx->codec_id));
        return AVERROR(EINVAL);
    }
    dec_desc = avcodec_descriptor_get(dec_ctx->codec_id);
    if (dec_desc && !(dec_desc->props & AV_CODEC_PROP_TEXT_SUB)) {
        av_log(ctx, AV_LOG_ERROR,
               "Only text based subtitles are currently supported\n");
        return AVERROR_PATCHWELCOME;
    }
    if (ass->charenc)
        av_dict_set(&codec_opts, "sub_charenc", ass->charenc, 0);
    ret = avcodec_open2(dec_ctx, dec, &codec_opts);
    if (ret < 0)
        goto end;
    if (ass->force_style) {
        char **list = NULL;
        char *temp = NULL;
        char *ptr = av_strtok(ass->force_style, ",", &temp);
        int i = 0;
        while (ptr) {
            av_dynarray_add(&list, &i, ptr);
            if (!list) {
                ret = AVERROR(ENOMEM);
                goto end;
            }
            ptr = av_strtok(NULL, ",", &temp);
        }
        av_dynarray_add(&list, &i, NULL);
        if (!list) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
        ass_set_style_overrides(ass->library, list);
        av_free(list);
    }
    
    if (dec_ctx->subtitle_header)
        ass_process_codec_private(ass->track,
                                  dec_ctx->subtitle_header,
                                  dec_ctx->subtitle_header_size);
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    while (av_read_frame(fmt, &pkt) >= 0) {
        int i, got_subtitle;
        AVSubtitle sub = {0};
        if (pkt.stream_index == sid) {
            ret = avcodec_decode_subtitle2(dec_ctx, &sub, &got_subtitle, &pkt);
            if (ret < 0) {
                av_log(ctx, AV_LOG_WARNING, "Error decoding: %s (ignored)\n",
                       av_err2str(ret));
            } else if (got_subtitle) {
                for (i = 0; i < sub.num_rects; i++) {
                    char *ass_line = sub.rects[i]->ass;
                    if (!ass_line)
                        break;
                    ass_process_data(ass->track, ass_line, strlen(ass_line));
                }
            }
        }
        av_free_packet(&pkt);
        avsubtitle_free(&sub);
    }
end:
    av_dict_free(&codec_opts);
    if (dec_ctx)
        avcodec_close(dec_ctx);
    if (fmt)
        avformat_close_input(&fmt);
    return ret;
}
