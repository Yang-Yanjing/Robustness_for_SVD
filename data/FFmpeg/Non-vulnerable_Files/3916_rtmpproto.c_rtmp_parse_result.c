static int rtmp_parse_result(URLContext *s, RTMPContext *rt, RTMPPacket *pkt)
{
    int ret;
#ifdef DEBUG
    ff_rtmp_packet_dump(s, pkt);
#endif
    switch (pkt->type) {
    case RTMP_PT_BYTES_READ:
        av_log(s, AV_LOG_TRACE, "received bytes read report\n");
        break;
    case RTMP_PT_CHUNK_SIZE:
        if ((ret = handle_chunk_size(s, pkt)) < 0)
            return ret;
        break;
    case RTMP_PT_PING:
        if ((ret = handle_ping(s, pkt)) < 0)
            return ret;
        break;
    case RTMP_PT_CLIENT_BW:
        if ((ret = handle_client_bw(s, pkt)) < 0)
            return ret;
        break;
    case RTMP_PT_SERVER_BW:
        if ((ret = handle_server_bw(s, pkt)) < 0)
            return ret;
        break;
    case RTMP_PT_INVOKE:
        if ((ret = handle_invoke(s, pkt)) < 0)
            return ret;
        break;
    case RTMP_PT_VIDEO:
    case RTMP_PT_AUDIO:
    case RTMP_PT_METADATA:
    case RTMP_PT_NOTIFY:
        
        break;
    default:
        av_log(s, AV_LOG_VERBOSE, "Unknown packet type received 0x%02X\n", pkt->type);
        break;
    }
    return 0;
}
