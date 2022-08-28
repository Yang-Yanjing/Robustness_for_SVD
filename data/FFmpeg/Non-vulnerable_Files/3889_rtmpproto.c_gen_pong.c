static int gen_pong(URLContext *s, RTMPContext *rt, RTMPPacket *ppkt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if (ppkt->size < 6) {
        av_log(s, AV_LOG_ERROR, "Too short ping packet (%d)\n",
               ppkt->size);
        return AVERROR_INVALIDDATA;
    }
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL, RTMP_PT_PING,
                                     ppkt->timestamp + 1, 6)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be16(&p, 7);
    bytestream_put_be32(&p, AV_RB32(ppkt->data+2));
    return rtmp_send_packet(rt, &pkt, 0);
}
