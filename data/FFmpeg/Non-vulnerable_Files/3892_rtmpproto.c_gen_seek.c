static int gen_seek(URLContext *s, RTMPContext *rt, int64_t timestamp)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Sending seek command for timestamp %"PRId64"\n",
           timestamp);
    if ((ret = ff_rtmp_packet_create(&pkt, 3, RTMP_PT_INVOKE, 0, 26)) < 0)
        return ret;
    pkt.extra = rt->stream_id;
    p = pkt.data;
    ff_amf_write_string(&p, "seek");
    ff_amf_write_number(&p, 0); 
    ff_amf_write_null(&p); 
    ff_amf_write_number(&p, timestamp); 
    return rtmp_send_packet(rt, &pkt, 1);
}
