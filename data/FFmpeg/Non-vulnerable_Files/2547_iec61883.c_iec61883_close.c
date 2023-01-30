static int iec61883_close(AVFormatContext *context)
{
    struct iec61883_data *dv = context->priv_data;
#if THREADS
    dv->thread_loop = 0;
    pthread_join(dv->receive_task_thread, NULL);
    pthread_cond_destroy(&dv->cond);
    pthread_mutex_destroy(&dv->mutex);
#endif
    if (CONFIG_MPEGTS_DEMUXER && dv->type == IEC61883_HDV) {
        iec61883_mpeg2_recv_stop(dv->iec61883_mpeg2);
        iec61883_mpeg2_close(dv->iec61883_mpeg2);
        avpriv_mpegts_parse_close(dv->mpeg_demux);
    } else {
        iec61883_dv_fb_stop(dv->iec61883_dv);
        iec61883_dv_fb_close(dv->iec61883_dv);
    }
    while (dv->queue_first) {
        DVPacket *packet = dv->queue_first;
        dv->queue_first = packet->next;
        av_freep(&packet->buf);
        av_freep(&packet);
    }
    iec61883_cmp_disconnect(dv->raw1394, dv->node, dv->output_port,
                            raw1394_get_local_id(dv->raw1394),
                            dv->input_port, dv->channel, dv->bandwidth);
    raw1394_destroy_handle(dv->raw1394);
    return 0;
}
