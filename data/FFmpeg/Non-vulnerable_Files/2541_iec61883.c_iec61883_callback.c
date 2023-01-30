static int iec61883_callback(unsigned char *data, int length,
                             int complete, void *callback_data)
{
    struct iec61883_data *dv = callback_data;
    DVPacket *packet;
    int ret;
#if THREADS
    pthread_mutex_lock(&dv->mutex);
#endif
    if (dv->packets >= dv->max_packets) {
        av_log(NULL, AV_LOG_ERROR, "DV packet queue overrun, dropping.\n");
        ret = 0;
        goto exit;
    }
    packet = av_mallocz(sizeof(*packet));
    if (!packet) {
        ret = -1;
        goto exit;
    }
    packet->buf = av_malloc(length);
    if (!packet->buf) {
        ret = -1;
        goto exit;
    }
    packet->len = length;
    memcpy(packet->buf, data, length);
    if (dv->queue_first) {
        dv->queue_last->next = packet;
        dv->queue_last = packet;
    } else {
        dv->queue_first = packet;
        dv->queue_last = packet;
    }
    dv->packets++;
    ret = 0;
exit:
#if THREADS
    pthread_cond_broadcast(&dv->cond);
    pthread_mutex_unlock(&dv->mutex);
#endif
    return ret;
}
