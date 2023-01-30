static int gen_publish(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Sending publish command for '%s'\n", rt->playpath);
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SOURCE_CHANNEL, RTMP_PT_INVOKE,
                                     0, 30 + strlen(rt->playpath))) < 0)
        return ret;
    pkt.extra = rt->stream_id;
    p = pkt.data;
    ff_amf_write_string(&p, "publish");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_null(&p);
    ff_amf_write_string(&p, rt->playpath);
    ff_amf_write_string(&p, "live");
    return rtmp_send_packet(rt, &pkt, 1);
}
