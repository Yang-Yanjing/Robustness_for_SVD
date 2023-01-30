static int gen_swf_verification(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Sending SWF verification...\n");
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL, RTMP_PT_PING,
                                     0, 44)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be16(&p, 27);
    memcpy(p, rt->swfverification, 42);
    return rtmp_send_packet(rt, &pkt, 0);
}
