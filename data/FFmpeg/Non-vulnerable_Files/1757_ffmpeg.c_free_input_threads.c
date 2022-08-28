static void free_input_threads(void)
{
    int i;
    for (i = 0; i < nb_input_files; i++) {
        InputFile *f = input_files[i];
        AVPacket pkt;
        if (!f || !f->in_thread_queue)
            continue;
        av_thread_message_queue_set_err_send(f->in_thread_queue, AVERROR_EOF);
        while (av_thread_message_queue_recv(f->in_thread_queue, &pkt, 0) >= 0)
            av_packet_unref(&pkt);
        pthread_join(f->thread, NULL);
        f->joined = 1;
        av_thread_message_queue_free(&f->in_thread_queue);
    }
}
