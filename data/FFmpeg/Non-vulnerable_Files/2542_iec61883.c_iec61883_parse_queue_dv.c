static int iec61883_parse_queue_dv(struct iec61883_data *dv, AVPacket *pkt)
{
    DVPacket *packet;
    int size;
    size = avpriv_dv_get_packet(dv->dv_demux, pkt);
    if (size > 0)
        return size;
    packet = dv->queue_first;
    if (!packet)
        return -1;
    size = avpriv_dv_produce_packet(dv->dv_demux, pkt,
                                    packet->buf, packet->len, -1);
#if FF_API_DESTRUCT_PACKET
FF_DISABLE_DEPRECATION_WARNINGS
    pkt->destruct = av_destruct_packet;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    dv->queue_first = packet->next;
    av_free(packet);
    dv->packets--;
    if (size > 0)
        return size;
    return -1;
}
