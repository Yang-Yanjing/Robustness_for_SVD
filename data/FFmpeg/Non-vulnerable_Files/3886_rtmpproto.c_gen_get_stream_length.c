static int gen_get_stream_length(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SOURCE_CHANNEL, RTMP_PT_INVOKE,
                                     0, 31 + strlen(rt->playpath))) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "getStreamLength");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_null(&p);
    ff_amf_write_string(&p, rt->playpath);
    return rtmp_send_packet(rt, &pkt, 1);
}
