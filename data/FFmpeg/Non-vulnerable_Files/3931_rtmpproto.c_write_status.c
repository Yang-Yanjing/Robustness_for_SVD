static int write_status(URLContext *s, RTMPPacket *pkt,
                        const char *status, const char *filename)
{
    RTMPContext *rt = s->priv_data;
    RTMPPacket spkt = { 0 };
    char statusmsg[128];
    uint8_t *pp;
    int ret;
    if ((ret = ff_rtmp_packet_create(&spkt, RTMP_SYSTEM_CHANNEL,
                                     RTMP_PT_INVOKE, 0,
                                     RTMP_PKTDATA_DEFAULT_SIZE)) < 0) {
        av_log(s, AV_LOG_ERROR, "Unable to create response packet\n");
        return ret;
    }
    pp = spkt.data;
    spkt.extra = pkt->extra;
    ff_amf_write_string(&pp, "onStatus");
    ff_amf_write_number(&pp, 0);
    ff_amf_write_null(&pp);
    ff_amf_write_object_start(&pp);
    ff_amf_write_field_name(&pp, "level");
    ff_amf_write_string(&pp, "status");
    ff_amf_write_field_name(&pp, "code");
    ff_amf_write_string(&pp, status);
    ff_amf_write_field_name(&pp, "description");
    snprintf(statusmsg, sizeof(statusmsg),
             "%s is now published", filename);
    ff_amf_write_string(&pp, statusmsg);
    ff_amf_write_field_name(&pp, "details");
    ff_amf_write_string(&pp, filename);
    ff_amf_write_field_name(&pp, "clientid");
    snprintf(statusmsg, sizeof(statusmsg), "%s", LIBAVFORMAT_IDENT);
    ff_amf_write_string(&pp, statusmsg);
    ff_amf_write_object_end(&pp);
    spkt.size = pp - spkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &spkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&spkt);
    return ret;
}
