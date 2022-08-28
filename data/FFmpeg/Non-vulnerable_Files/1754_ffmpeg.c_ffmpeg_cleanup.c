static void ffmpeg_cleanup(int ret)
{
    int i, j;
    if (do_benchmark) {
        int maxrss = getmaxrss() / 1024;
        av_log(NULL, AV_LOG_INFO, "bench: maxrss=%ikB\n", maxrss);
    }
    for (i = 0; i < nb_filtergraphs; i++) {
        FilterGraph *fg = filtergraphs[i];
        avfilter_graph_free(&fg->graph);
        for (j = 0; j < fg->nb_inputs; j++) {
            while (av_fifo_size(fg->inputs[j]->frame_queue)) {
                AVFrame *frame;
                av_fifo_generic_read(fg->inputs[j]->frame_queue, &frame,
                                     sizeof(frame), NULL);
                av_frame_free(&frame);
            }
            av_fifo_free(fg->inputs[j]->frame_queue);
            if (fg->inputs[j]->ist->sub2video.sub_queue) {
                while (av_fifo_size(fg->inputs[j]->ist->sub2video.sub_queue)) {
                    AVSubtitle sub;
                    av_fifo_generic_read(fg->inputs[j]->ist->sub2video.sub_queue,
                                         &sub, sizeof(sub), NULL);
                    avsubtitle_free(&sub);
                }
                av_fifo_free(fg->inputs[j]->ist->sub2video.sub_queue);
            }
            av_buffer_unref(&fg->inputs[j]->hw_frames_ctx);
            av_freep(&fg->inputs[j]->name);
            av_freep(&fg->inputs[j]);
        }
        av_freep(&fg->inputs);
        for (j = 0; j < fg->nb_outputs; j++) {
            av_freep(&fg->outputs[j]->name);
            av_freep(&fg->outputs[j]->formats);
            av_freep(&fg->outputs[j]->channel_layouts);
            av_freep(&fg->outputs[j]->sample_rates);
            av_freep(&fg->outputs[j]);
        }
        av_freep(&fg->outputs);
        av_freep(&fg->graph_desc);
        av_freep(&filtergraphs[i]);
    }
    av_freep(&filtergraphs);
    av_freep(&subtitle_out);
    
    for (i = 0; i < nb_output_files; i++) {
        OutputFile *of = output_files[i];
        AVFormatContext *s;
        if (!of)
            continue;
        s = of->ctx;
        if (s && s->oformat && !(s->oformat->flags & AVFMT_NOFILE))
            avio_closep(&s->pb);
        avformat_free_context(s);
        av_dict_free(&of->opts);
        av_freep(&output_files[i]);
    }
    for (i = 0; i < nb_output_streams; i++) {
        OutputStream *ost = output_streams[i];
        if (!ost)
            continue;
        for (j = 0; j < ost->nb_bitstream_filters; j++)
            av_bsf_free(&ost->bsf_ctx[j]);
        av_freep(&ost->bsf_ctx);
        av_freep(&ost->bsf_extradata_updated);
        av_frame_free(&ost->filtered_frame);
        av_frame_free(&ost->last_frame);
        av_dict_free(&ost->encoder_opts);
        av_parser_close(ost->parser);
        avcodec_free_context(&ost->parser_avctx);
        av_freep(&ost->forced_keyframes);
        av_expr_free(ost->forced_keyframes_pexpr);
        av_freep(&ost->avfilter);
        av_freep(&ost->logfile_prefix);
        av_freep(&ost->audio_channels_map);
        ost->audio_channels_mapped = 0;
        av_dict_free(&ost->sws_dict);
        avcodec_free_context(&ost->enc_ctx);
        avcodec_parameters_free(&ost->ref_par);
        if (ost->muxing_queue) {
            while (av_fifo_size(ost->muxing_queue)) {
                AVPacket pkt;
                av_fifo_generic_read(ost->muxing_queue, &pkt, sizeof(pkt), NULL);
                av_packet_unref(&pkt);
            }
            av_fifo_freep(&ost->muxing_queue);
        }
        av_freep(&output_streams[i]);
    }
#if HAVE_PTHREADS
    free_input_threads();
#endif
    for (i = 0; i < nb_input_files; i++) {
        avformat_close_input(&input_files[i]->ctx);
        av_freep(&input_files[i]);
    }
    for (i = 0; i < nb_input_streams; i++) {
        InputStream *ist = input_streams[i];
        av_frame_free(&ist->decoded_frame);
        av_frame_free(&ist->filter_frame);
        av_dict_free(&ist->decoder_opts);
        avsubtitle_free(&ist->prev_sub.subtitle);
        av_frame_free(&ist->sub2video.frame);
        av_freep(&ist->filters);
        av_freep(&ist->hwaccel_device);
        av_freep(&ist->dts_buffer);
        avcodec_free_context(&ist->dec_ctx);
        av_freep(&input_streams[i]);
    }
    if (vstats_file) {
        if (fclose(vstats_file))
            av_log(NULL, AV_LOG_ERROR,
                   "Error closing vstats file, loss of information possible: %s\n",
                   av_err2str(AVERROR(errno)));
    }
    av_freep(&vstats_filename);
    av_freep(&input_streams);
    av_freep(&input_files);
    av_freep(&output_streams);
    av_freep(&output_files);
    uninit_opts();
    avformat_network_deinit();
    if (received_sigterm) {
        av_log(NULL, AV_LOG_INFO, "Exiting normally, received signal %d.\n",
               (int) received_sigterm);
    } else if (ret && atomic_load(&transcode_init_done)) {
        av_log(NULL, AV_LOG_INFO, "Conversion failed!\n");
    }
    term_exit();
    ffmpeg_exited = 1;
}
