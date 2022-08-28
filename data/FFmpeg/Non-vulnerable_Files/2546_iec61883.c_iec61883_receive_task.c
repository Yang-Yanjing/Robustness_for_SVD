static void *iec61883_receive_task(void *opaque)
{
    struct iec61883_data *dv = (struct iec61883_data *)opaque;
    int result;
#if THREADS
    while (dv->thread_loop)
#endif
    {
        while ((result = poll(&dv->raw1394_poll, 1, 200)) < 0) {
            if (!(errno == EAGAIN || errno == EINTR)) {
                av_log(NULL, AV_LOG_ERROR, "Raw1394 poll error occurred.\n");
                dv->receive_error = AVERROR(EIO);
                return NULL;
            }
        }
        if (result > 0 && ((dv->raw1394_poll.revents & POLLIN)
                           || (dv->raw1394_poll.revents & POLLPRI))) {
            dv->receiving = 1;
            raw1394_loop_iterate(dv->raw1394);
        } else if (dv->receiving) {
            av_log(NULL, AV_LOG_ERROR, "No more input data available\n");
#if THREADS
            pthread_mutex_lock(&dv->mutex);
            dv->eof = 1;
            pthread_cond_broadcast(&dv->cond);
            pthread_mutex_unlock(&dv->mutex);
#else
            dv->eof = 1;
#endif
            return NULL;
        }
    }
    return NULL;
}
