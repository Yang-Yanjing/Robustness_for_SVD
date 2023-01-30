static int transcode(void)
{
    int ret, i;
    AVFormatContext *os;
    OutputStream *ost;
    InputStream *ist;
    int64_t timer_start;
    int64_t total_packets_written = 0;
    ret = transcode_init();
    if (ret < 0)
        goto fail;
    if (stdin_interaction) {
        av_log(NULL, AV_LOG_INFO, "Press [q] to stop, [?] for help\n");
    }
    timer_start = av_gettime_relative();
#if HAVE_PTHREADS
    if ((ret = init_input_threads()) < 0)
        goto fail;
#endif
    while (!received_sigterm) {
        int64_t cur_time= av_gettime_relative();
        
        if (stdin_interaction)
            if (check_keyboard_interaction(cur_time) < 0)
                break;
        
        if (!need_output()) {
            av_log(NULL, AV_LOG_VERBOSE, "No more output streams to write to, finishing.\n");
            break;
        }
        ret = transcode_step();
        if (ret < 0 && ret != AVERROR_EOF) {
            char errbuf[128];
            av_strerror(ret, errbuf, sizeof(errbuf));
            av_log(NULL, AV_LOG_ERROR, "Error while filtering: %s\n", errbuf);
            break;
        }
        
        print_report(0, timer_start, cur_time);
    }
#if HAVE_PTHREADS
    free_input_threads();
#endif
    
    for (i = 0; i < nb_input_streams; i++) {
        ist = input_streams[i];
        if (!input_files[ist->file_index]->eof_reached && ist->decoding_needed) {
            process_input_packet(ist, NULL, 0);
        }
    }
    flush_encoders();
    term_exit();
    
    for (i = 0; i < nb_output_files; i++) {
        os = output_files[i]->ctx;
        if (!output_files[i]->header_written) {
            av_log(NULL, AV_LOG_ERROR,
                   "Nothing was written into output file %d (%s), because "
                   "at least one of its streams received no packets.\n",
                   i, os->filename);
            continue;
        }
        if ((ret = av_write_trailer(os)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error writing trailer of %s: %s\n", os->filename, av_err2str(ret));
            if (exit_on_error)
                exit_program(1);
        }
    }
    
    print_report(1, timer_start, av_gettime_relative());
    
    for (i = 0; i < nb_output_streams; i++) {
        ost = output_streams[i];
        if (ost->encoding_needed) {
            av_freep(&ost->enc_ctx->stats_in);
        }
        total_packets_written += ost->packets_written;
    }
    if (!total_packets_written && (abort_on_flags & ABORT_ON_FLAG_EMPTY_OUTPUT)) {
        av_log(NULL, AV_LOG_FATAL, "Empty output\n");
        exit_program(1);
    }
    
    for (i = 0; i < nb_input_streams; i++) {
        ist = input_streams[i];
        if (ist->decoding_needed) {
            avcodec_close(ist->dec_ctx);
            if (ist->hwaccel_uninit)
                ist->hwaccel_uninit(ist->dec_ctx);
        }
    }
    av_buffer_unref(&hw_device_ctx);
    
    ret = 0;
 fail:
#if HAVE_PTHREADS
    free_input_threads();
#endif
    if (output_streams) {
        for (i = 0; i < nb_output_streams; i++) {
            ost = output_streams[i];
            if (ost) {
                if (ost->logfile) {
                    if (fclose(ost->logfile))
                        av_log(NULL, AV_LOG_ERROR,
                               "Error closing logfile, loss of information possible: %s\n",
                               av_err2str(AVERROR(errno)));
                    ost->logfile = NULL;
                }
                av_freep(&ost->forced_kf_pts);
                av_freep(&ost->apad);
                av_freep(&ost->disposition);
                av_dict_free(&ost->encoder_opts);
                av_dict_free(&ost->sws_dict);
                av_dict_free(&ost->swr_opts);
                av_dict_free(&ost->resample_opts);
            }
        }
    }
    return ret;
}
