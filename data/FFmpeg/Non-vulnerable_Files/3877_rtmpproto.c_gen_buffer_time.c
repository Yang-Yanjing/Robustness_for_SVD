static int gen_buffer_time(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL, RTMP_PT_PING,
                                     1, 10)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be16(&p, 3);
    bytestream_put_be32(&p, rt->stream_id);
    bytestream_put_be32(&p, rt->client_buffer_time);
    return rtmp_send_packet(rt, &pkt, 0);
}
