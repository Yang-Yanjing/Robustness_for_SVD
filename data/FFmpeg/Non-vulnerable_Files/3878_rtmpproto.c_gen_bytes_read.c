static int gen_bytes_read(URLContext *s, RTMPContext *rt, uint32_t ts)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL, RTMP_PT_BYTES_READ,
                                     ts, 4)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be32(&p, rt->bytes_read);
    return rtmp_send_packet(rt, &pkt, 0);
}
