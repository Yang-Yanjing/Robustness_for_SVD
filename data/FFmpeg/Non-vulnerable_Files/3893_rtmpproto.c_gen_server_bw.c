static int gen_server_bw(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL, RTMP_PT_SERVER_BW,
                                     0, 4)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be32(&p, rt->server_bw);
    return rtmp_send_packet(rt, &pkt, 0);
}
