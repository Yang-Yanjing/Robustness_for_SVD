static int read_connect(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt = { 0 };
    uint8_t *p;
    const uint8_t *cp;
    int ret;
    char command[64];
    int stringlen;
    double seqnum;
    uint8_t tmpstr[256];
    GetByteContext gbc;
    if ((ret = ff_rtmp_packet_read(rt->stream, &pkt, rt->in_chunk_size,
                                   &rt->prev_pkt[0], &rt->nb_prev_pkt[0])) < 0)
        return ret;
    if (pkt.type == RTMP_PT_CHUNK_SIZE) {
        if ((ret = handle_chunk_size(s, &pkt)) < 0)
            return ret;
        ff_rtmp_packet_destroy(&pkt);
        if ((ret = ff_rtmp_packet_read(rt->stream, &pkt, rt->in_chunk_size,
                                       &rt->prev_pkt[0], &rt->nb_prev_pkt[0])) < 0)
            return ret;
    }
    cp = pkt.data;
    bytestream2_init(&gbc, cp, pkt.size);
    if (ff_amf_read_string(&gbc, command, sizeof(command), &stringlen)) {
        av_log(s, AV_LOG_ERROR, "Unable to read command string\n");
        ff_rtmp_packet_destroy(&pkt);
        return AVERROR_INVALIDDATA;
    }
    if (strcmp(command, "connect")) {
        av_log(s, AV_LOG_ERROR, "Expecting connect, got %s\n", command);
        ff_rtmp_packet_destroy(&pkt);
        return AVERROR_INVALIDDATA;
    }
    ret = ff_amf_read_number(&gbc, &seqnum);
    if (ret)
        av_log(s, AV_LOG_WARNING, "SeqNum not found\n");
    
    ret = ff_amf_get_field_value(gbc.buffer,
                                 gbc.buffer + bytestream2_get_bytes_left(&gbc),
                                 "app", tmpstr, sizeof(tmpstr));
    if (ret)
        av_log(s, AV_LOG_WARNING, "App field not found in connect\n");
    if (!ret && strcmp(tmpstr, rt->app))
        av_log(s, AV_LOG_WARNING, "App field don't match up: %s <-> %s\n",
               tmpstr, rt->app);
    ff_rtmp_packet_destroy(&pkt);
    
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL,
                                     RTMP_PT_SERVER_BW, 0, 4)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be32(&p, rt->server_bw);
    pkt.size = p - pkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    if (ret < 0)
        return ret;
    
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL,
                                     RTMP_PT_CLIENT_BW, 0, 5)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be32(&p, rt->server_bw);
    bytestream_put_byte(&p, 2); 
    pkt.size = p - pkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    if (ret < 0)
        return ret;
    
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_NETWORK_CHANNEL,
                                     RTMP_PT_PING, 0, 6)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be16(&p, 0); 
    bytestream_put_be32(&p, 0);
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    if (ret < 0)
        return ret;
    
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL,
                                     RTMP_PT_CHUNK_SIZE, 0, 4)) < 0)
        return ret;
    p = pkt.data;
    bytestream_put_be32(&p, rt->out_chunk_size);
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    if (ret < 0)
        return ret;
    
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL,
                                     RTMP_PT_INVOKE, 0,
                                     RTMP_PKTDATA_DEFAULT_SIZE)) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "_result");
    ff_amf_write_number(&p, seqnum);
    ff_amf_write_object_start(&p);
    ff_amf_write_field_name(&p, "fmsVer");
    ff_amf_write_string(&p, "FMS/3,0,1,123");
    ff_amf_write_field_name(&p, "capabilities");
    ff_amf_write_number(&p, 31);
    ff_amf_write_object_end(&p);
    ff_amf_write_object_start(&p);
    ff_amf_write_field_name(&p, "level");
    ff_amf_write_string(&p, "status");
    ff_amf_write_field_name(&p, "code");
    ff_amf_write_string(&p, "NetConnection.Connect.Success");
    ff_amf_write_field_name(&p, "description");
    ff_amf_write_string(&p, "Connection succeeded.");
    ff_amf_write_field_name(&p, "objectEncoding");
    ff_amf_write_number(&p, 0);
    ff_amf_write_object_end(&p);
    pkt.size = p - pkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    if (ret < 0)
        return ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL,
                                     RTMP_PT_INVOKE, 0, 30)) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "onBWDone");
    ff_amf_write_number(&p, 0);
    ff_amf_write_null(&p);
    ff_amf_write_number(&p, 8192);
    pkt.size = p - pkt.data;
    ret = ff_rtmp_packet_write(rt->stream, &pkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&pkt);
    return ret;
}
