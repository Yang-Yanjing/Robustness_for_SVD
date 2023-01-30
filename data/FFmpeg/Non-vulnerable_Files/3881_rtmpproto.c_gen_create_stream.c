static int gen_create_stream(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Creating stream...\n");
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL, RTMP_PT_INVOKE,
                                     0, 25)) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "createStream");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_null(&p);
    return rtmp_send_packet(rt, &pkt, 1);
}
