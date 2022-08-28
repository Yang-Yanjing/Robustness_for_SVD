static int init_input_threads(void)
{
    int i, ret;
    if (nb_input_files == 1)
        return 0;
    for (i = 0; i < nb_input_files; i++) {
        InputFile *f = input_files[i];
        if (f->ctx->pb ? !f->ctx->pb->seekable :
            strcmp(f->ctx->iformat->name, "lavfi"))
            f->non_blocking = 1;
        ret = av_thread_message_queue_alloc(&f->in_thread_queue,
                                            f->thread_queue_size, sizeof(AVPacket));
        if (ret < 0)
            return ret;
        if ((ret = pthread_create(&f->thread, NULL, input_thread, f))) {
            av_log(NULL, AV_LOG_ERROR, "pthread_create failed: %s. Try to increase `ulimit -v` or decrease `ulimit -s`.\n", strerror(ret));
            av_thread_message_queue_free(&f->in_thread_queue);
            return AVERROR(ret);
        }
    }
    return 0;
}
