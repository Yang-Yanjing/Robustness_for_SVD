static void *input_thread(void *arg)
{
    InputFile *f = arg;
    unsigned flags = f->non_blocking ? AV_THREAD_MESSAGE_NONBLOCK : 0;
    int ret = 0;
    while (1) {
        AVPacket pkt;
        ret = av_read_frame(f->ctx, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            av_usleep(10000);
            continue;
        }
        if (ret < 0) {
            av_thread_message_queue_set_err_recv(f->in_thread_queue, ret);
            break;
        }
        ret = av_thread_message_queue_send(f->in_thread_queue, &pkt, flags);
        if (flags && ret == AVERROR(EAGAIN)) {
            flags = 0;
            ret = av_thread_message_queue_send(f->in_thread_queue, &pkt, flags);
            av_log(f->ctx, AV_LOG_WARNING,
                   "Thread message queue blocking; consider raising the "
                   "thread_queue_size option (current value: %d)\n",
                   f->thread_queue_size);
        }
        if (ret < 0) {
            if (ret != AVERROR_EOF)
                av_log(f->ctx, AV_LOG_ERROR,
                       "Unable to send packet to main thread: %s\n",
                       av_err2str(ret));
            av_packet_unref(&pkt);
            av_thread_message_queue_set_err_recv(f->in_thread_queue, ret);
            break;
        }
    }
    return NULL;
}
