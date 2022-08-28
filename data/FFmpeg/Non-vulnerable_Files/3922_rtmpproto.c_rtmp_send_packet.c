static int rtmp_send_packet(RTMPContext *rt, RTMPPacket *pkt, int track)
{
    int ret;
    if (pkt->type == RTMP_PT_INVOKE && track) {
        GetByteContext gbc;
        char name[128];
        double pkt_id;
        int len;
        bytestream2_init(&gbc, pkt->data, pkt->size);
        if ((ret = ff_amf_read_string(&gbc, name, sizeof(name), &len)) < 0)
            goto fail;
        if ((ret = ff_amf_read_number(&gbc, &pkt_id)) < 0)
            goto fail;
        if ((ret = add_tracked_method(rt, name, pkt_id)) < 0)
            goto fail;
    }
    ret = ff_rtmp_packet_write(rt->stream, pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
fail:
    ff_rtmp_packet_destroy(pkt);
    return ret;
}
