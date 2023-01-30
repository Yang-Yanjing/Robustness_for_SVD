static int gen_fcsubscribe_stream(URLContext *s, RTMPContext *rt,
                                  const char *subscribe)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL, RTMP_PT_INVOKE,
                                     0, 27 + strlen(subscribe))) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "FCSubscribe");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_null(&p);
    ff_amf_write_string(&p, subscribe);
    return rtmp_send_packet(rt, &pkt, 1);
}
