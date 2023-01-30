static int iec61883_parse_queue_hdv(struct iec61883_data *dv, AVPacket *pkt)
{
    DVPacket *packet;
    int size;
    while (dv->queue_first) {
        packet = dv->queue_first;
        size = avpriv_mpegts_parse_packet(dv->mpeg_demux, pkt, packet->buf,
                                          packet->len);
        dv->queue_first = packet->next;
        av_freep(&packet->buf);
        av_freep(&packet);
        dv->packets--;
        if (size > 0)
            return size;
    }
    return -1;
}
