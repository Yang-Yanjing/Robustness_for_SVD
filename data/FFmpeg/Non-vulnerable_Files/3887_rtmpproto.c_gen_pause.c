static int gen_pause(URLContext *s, RTMPContext *rt, int pause, uint32_t timestamp)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Sending pause command for timestamp %d\n",
           timestamp);
    if ((ret = ff_rtmp_packet_create(&pkt, 3, RTMP_PT_INVOKE, 0, 29)) < 0)
        return ret;
    pkt.extra = rt->stream_id;
    p = pkt.data;
    ff_amf_write_string(&p, "pause");
    ff_amf_write_number(&p, 0); 
    ff_amf_write_null(&p); 
    ff_amf_write_bool(&p, pause); 
    ff_amf_write_number(&p, timestamp); 
    return rtmp_send_packet(rt, &pkt, 1);
}
