static int write_begin(URLContext *s)
{
    RTMPContext *rt = s->priv_data;
    PutByteContext pbc;
    RTMPPacket spkt = { 0 };
    int ret;
    
    if ((ret = ff_rtmp_packet_create(&spkt, RTMP_NETWORK_CHANNEL,
                                     RTMP_PT_PING, 0, 6)) < 0) {
        av_log(s, AV_LOG_ERROR, "Unable to create response packet\n");
        return ret;
    }
    bytestream2_init_writer(&pbc, spkt.data, spkt.size);
    bytestream2_put_be16(&pbc, 0);          
    bytestream2_put_be32(&pbc, rt->nb_streamid);
    ret = ff_rtmp_packet_write(rt->stream, &spkt, rt->out_chunk_size,
                               &rt->prev_pkt[1], &rt->nb_prev_pkt[1]);
    ff_rtmp_packet_destroy(&spkt);
    return ret;
}
