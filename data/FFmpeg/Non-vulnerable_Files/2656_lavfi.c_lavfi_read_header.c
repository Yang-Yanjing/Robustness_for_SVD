av_cold static int lavfi_read_header(AVFormatContext *avctx)
{
    LavfiContext *lavfi = avctx->priv_data;
    AVFilterInOut *input_links = NULL, *output_links = NULL, *inout;
    AVFilter *buffersink, *abuffersink;
    int *pix_fmts = create_all_formats(AV_PIX_FMT_NB);
    enum AVMediaType type;
    int ret = 0, i, n;
#define FAIL(ERR) { ret = ERR; goto end; }
    if (!pix_fmts)
        FAIL(AVERROR(ENOMEM));
    avfilter_register_all();
    buffersink = avfilter_get_by_name("buffersink");
    abuffersink = avfilter_get_by_name("abuffersink");
    if (lavfi->graph_filename && lavfi->graph_str) {
        av_log(avctx, AV_LOG_ERROR,
               "Only one of the graph or graph_file options must be specified\n");
        FAIL(AVERROR(EINVAL));
    }
    if (lavfi->graph_filename) {
        AVBPrint graph_file_pb;
        AVIOContext *avio = NULL;
        ret = avio_open(&avio, lavfi->graph_filename, AVIO_FLAG_READ);
        if (ret < 0)
            goto end;
        av_bprint_init(&graph_file_pb, 0, AV_BPRINT_SIZE_UNLIMITED);
        ret = avio_read_to_bprint(avio, &graph_file_pb, INT_MAX);
        avio_closep(&avio);
        av_bprint_chars(&graph_file_pb, '\0', 1);
        if (!ret && !av_bprint_is_complete(&graph_file_pb))
            ret = AVERROR(ENOMEM);
        if (ret) {
            av_bprint_finalize(&graph_file_pb, NULL);
            goto end;
        }
        if ((ret = av_bprint_finalize(&graph_file_pb, &lavfi->graph_str)))
            goto end;
    }
    if (!lavfi->graph_str)
        lavfi->graph_str = av_strdup(avctx->filename);
    
    if (!(lavfi->graph = avfilter_graph_alloc()))
        FAIL(AVERROR(ENOMEM));
    if ((ret = avfilter_graph_parse_ptr(lavfi->graph, lavfi->graph_str,
                                    &input_links, &output_links, avctx)) < 0)
        goto end;
    if (input_links) {
        av_log(avctx, AV_LOG_ERROR,
               "Open inputs in the filtergraph are not acceptable\n");
        FAIL(AVERROR(EINVAL));
    }
    
    for (n = 0, inout = output_links; inout; n++, inout = inout->next);
    lavfi->nb_sinks = n;
    if (!(lavfi->sink_stream_map = av_malloc(sizeof(int) * n)))
        FAIL(AVERROR(ENOMEM));
    if (!(lavfi->sink_eof = av_mallocz(sizeof(int) * n)))
        FAIL(AVERROR(ENOMEM));
    if (!(lavfi->stream_sink_map = av_malloc(sizeof(int) * n)))
        FAIL(AVERROR(ENOMEM));
    if (!(lavfi->sink_stream_subcc_map = av_malloc(sizeof(int) * n)))
        FAIL(AVERROR(ENOMEM));
    for (i = 0; i < n; i++)
        lavfi->stream_sink_map[i] = -1;
    

    for (i = 0, inout = output_links; inout; i++, inout = inout->next) {
        int stream_idx = 0, suffix = 0, use_subcc = 0;
        sscanf(inout->name, "out%n%d%n", &suffix, &stream_idx, &suffix);
        if (!suffix) {
            av_log(avctx,  AV_LOG_ERROR,
                   "Invalid outpad name '%s'\n", inout->name);
            FAIL(AVERROR(EINVAL));
        }
        if (inout->name[suffix]) {
            if (!strcmp(inout->name + suffix, "+subcc")) {
                use_subcc = 1;
            } else {
                av_log(avctx,  AV_LOG_ERROR,
                       "Invalid outpad suffix '%s'\n", inout->name);
                FAIL(AVERROR(EINVAL));
            }
        }
        if ((unsigned)stream_idx >= n) {
            av_log(avctx, AV_LOG_ERROR,
                   "Invalid index was specified in output '%s', "
                   "must be a non-negative value < %d\n",
                   inout->name, n);
            FAIL(AVERROR(EINVAL));
        }
        if (lavfi->stream_sink_map[stream_idx] != -1) {
            av_log(avctx,  AV_LOG_ERROR,
                   "An output with stream index %d was already specified\n",
                   stream_idx);
            FAIL(AVERROR(EINVAL));
        }
        lavfi->sink_stream_map[i] = stream_idx;
        lavfi->stream_sink_map[stream_idx] = i;
        lavfi->sink_stream_subcc_map[i] = !!use_subcc;
    }
    
    for (i = 0, inout = output_links; inout; i++, inout = inout->next) {
        AVStream *st;
        if (!(st = avformat_new_stream(avctx, NULL)))
            FAIL(AVERROR(ENOMEM));
        st->id = i;
    }
    
    lavfi->sinks = av_malloc_array(lavfi->nb_sinks, sizeof(AVFilterContext *));
    if (!lavfi->sinks)
        FAIL(AVERROR(ENOMEM));
    for (i = 0, inout = output_links; inout; i++, inout = inout->next) {
        AVFilterContext *sink;
        type = avfilter_pad_get_type(inout->filter_ctx->output_pads, inout->pad_idx);
        if (type == AVMEDIA_TYPE_VIDEO && ! buffersink ||
            type == AVMEDIA_TYPE_AUDIO && ! abuffersink) {
                av_log(avctx, AV_LOG_ERROR, "Missing required buffersink filter, aborting.\n");
                FAIL(AVERROR_FILTER_NOT_FOUND);
        }
        if (type == AVMEDIA_TYPE_VIDEO) {
            ret = avfilter_graph_create_filter(&sink, buffersink,
                                               inout->name, NULL,
                                               NULL, lavfi->graph);
            if (ret >= 0)
                ret = av_opt_set_int_list(sink, "pix_fmts", pix_fmts,  AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
            if (ret < 0)
                goto end;
        } else if (type == AVMEDIA_TYPE_AUDIO) {
            enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_U8,
                                                  AV_SAMPLE_FMT_S16,
                                                  AV_SAMPLE_FMT_S32,
                                                  AV_SAMPLE_FMT_FLT,
                                                  AV_SAMPLE_FMT_DBL, -1 };
            ret = avfilter_graph_create_filter(&sink, abuffersink,
                                               inout->name, NULL,
                                               NULL, lavfi->graph);
            if (ret >= 0)
                ret = av_opt_set_int_list(sink, "sample_fmts", sample_fmts,  AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
            if (ret < 0)
                goto end;
            ret = av_opt_set_int(sink, "all_channel_counts", 1,
                                 AV_OPT_SEARCH_CHILDREN);
            if (ret < 0)
                goto end;
        } else {
            av_log(avctx,  AV_LOG_ERROR,
                   "Output '%s' is not a video or audio output, not yet supported\n", inout->name);
            FAIL(AVERROR(EINVAL));
        }
        lavfi->sinks[i] = sink;
        if ((ret = avfilter_link(inout->filter_ctx, inout->pad_idx, sink, 0)) < 0)
            goto end;
    }
    
    if ((ret = avfilter_graph_config(lavfi->graph, avctx)) < 0)
        goto end;
    if (lavfi->dump_graph) {
        char *dump = avfilter_graph_dump(lavfi->graph, lavfi->dump_graph);
        fputs(dump, stderr);
        fflush(stderr);
        av_free(dump);
    }
    
    for (i = 0; i < lavfi->nb_sinks; i++) {
        AVFilterLink *link = lavfi->sinks[lavfi->stream_sink_map[i]]->inputs[0];
        AVStream *st = avctx->streams[i];
        st->codec->codec_type = link->type;
        avpriv_set_pts_info(st, 64, link->time_base.num, link->time_base.den);
        if (link->type == AVMEDIA_TYPE_VIDEO) {
            st->codec->codec_id   = AV_CODEC_ID_RAWVIDEO;
            st->codec->pix_fmt    = link->format;
            st->codec->time_base  = link->time_base;
            st->codec->width      = link->w;
            st->codec->height     = link->h;
            st       ->sample_aspect_ratio =
            st->codec->sample_aspect_ratio = link->sample_aspect_ratio;
            avctx->probesize = FFMAX(avctx->probesize,
                                     link->w * link->h *
                                     av_get_padded_bits_per_pixel(av_pix_fmt_desc_get(link->format)) *
                                     30);
        } else if (link->type == AVMEDIA_TYPE_AUDIO) {
            st->codec->codec_id    = av_get_pcm_codec(link->format, -1);
            st->codec->channels    = avfilter_link_get_channels(link);
            st->codec->sample_fmt  = link->format;
            st->codec->sample_rate = link->sample_rate;
            st->codec->time_base   = link->time_base;
            st->codec->channel_layout = link->channel_layout;
            if (st->codec->codec_id == AV_CODEC_ID_NONE)
                av_log(avctx, AV_LOG_ERROR,
                       "Could not find PCM codec for sample format %s.\n",
                       av_get_sample_fmt_name(link->format));
        }
    }
    if ((ret = create_subcc_streams(avctx)) < 0)
        goto end;
    if (!(lavfi->decoded_frame = av_frame_alloc()))
        FAIL(AVERROR(ENOMEM));
end:
    av_free(pix_fmts);
    avfilter_inout_free(&input_links);
    avfilter_inout_free(&output_links);
    if (ret < 0)
        lavfi_read_close(avctx);
    return ret;
}
