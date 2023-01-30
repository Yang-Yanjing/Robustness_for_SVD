int av_thread_message_queue_recv(AVThreadMessageQueue *mq,
                                 void *msg,
                                 unsigned flags)
{
#if HAVE_THREADS
    int ret;
    pthread_mutex_lock(&mq->lock);
    ret = av_thread_message_queue_recv_locked(mq, msg, flags);
    pthread_mutex_unlock(&mq->lock);
    return ret;
#else
    return AVERROR(ENOSYS);
#endif 
}
