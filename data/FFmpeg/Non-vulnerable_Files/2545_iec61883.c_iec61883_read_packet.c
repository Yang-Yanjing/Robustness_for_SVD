static int iec61883_read_packet(AVFormatContext *context, AVPacket *pkt)
{
    struct iec61883_data *dv = context->priv_data;
    int size;
    


#if THREADS
    pthread_mutex_lock(&dv->mutex);
    while ((size = dv->parse_queue(dv, pkt)) == -1)
        if (!dv->eof)
            pthread_cond_wait(&dv->cond, &dv->mutex);
        else
            break;
    pthread_mutex_unlock(&dv->mutex);
#else
    int result;
    while ((size = dv->parse_queue(dv, pkt)) == -1) {
        iec61883_receive_task((void *)dv);
        if (dv->receive_error)
            return dv->receive_error;
    }
#endif
    return size;
}
