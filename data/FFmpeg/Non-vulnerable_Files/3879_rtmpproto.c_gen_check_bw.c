static int gen_check_bw(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL, RTMP_PT_INVOKE,
                                     0, 21)) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "_checkbw");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_null(&p);
    return rtmp_send_packet(rt, &pkt, 1);
}
