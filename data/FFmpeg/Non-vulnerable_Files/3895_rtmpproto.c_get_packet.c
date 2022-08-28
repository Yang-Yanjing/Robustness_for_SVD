static int get_packet(URLContext *s, int for_header)
{
    RTMPContext *rt = s->priv_data;
    int ret;
    if (rt->state == STATE_STOPPED)
        return AVERROR_EOF;
    for (;;) {
        RTMPPacket rpkt = { 0 };
        if ((ret = ff_rtmp_packet_read(rt->stream, &rpkt,
                                       rt->in_chunk_size, &rt->prev_pkt[0],
                                       &rt->nb_prev_pkt[0])) <= 0) {
            if (ret == 0) {
                return AVERROR(EAGAIN);
            } else {
                return AVERROR(EIO);
            }
        }
        
        rt->last_timestamp = rpkt.timestamp;
        rt->bytes_read += ret;
        if (rt->bytes_read - rt->last_bytes_read > rt->client_report_size) {
            av_log(s, AV_LOG_DEBUG, "Sending bytes read report\n");
            if ((ret = gen_bytes_read(s, rt, rpkt.timestamp + 1)) < 0)
                return ret;
            rt->last_bytes_read = rt->bytes_read;
        }
        ret = rtmp_parse_result(s, rt, &rpkt);
        
        
        
        if (rt->state == STATE_SEEKING) {
            ff_rtmp_packet_destroy(&rpkt);
            
            
            continue;
        }
        if (ret < 0) {
            ff_rtmp_packet_destroy(&rpkt);
            return ret;
        }
        if (rt->do_reconnect && for_header) {
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        }
        if (rt->state == STATE_STOPPED) {
            ff_rtmp_packet_destroy(&rpkt);
            return AVERROR_EOF;
        }
        if (for_header && (rt->state == STATE_PLAYING    ||
                           rt->state == STATE_PUBLISHING ||
                           rt->state == STATE_SENDING    ||
                           rt->state == STATE_RECEIVING)) {
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        }
        if (!rpkt.size || !rt->is_input) {
            ff_rtmp_packet_destroy(&rpkt);
            continue;
        }
        if (rpkt.type == RTMP_PT_VIDEO || rpkt.type == RTMP_PT_AUDIO) {
            ret = append_flv_data(rt, &rpkt, 0);
            ff_rtmp_packet_destroy(&rpkt);
            return ret;
        } else if (rpkt.type == RTMP_PT_NOTIFY) {
            ret = handle_notify(s, &rpkt);
            ff_rtmp_packet_destroy(&rpkt);
            return ret;
        } else if (rpkt.type == RTMP_PT_METADATA) {
            ret = handle_metadata(rt, &rpkt);
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        }
        ff_rtmp_packet_destroy(&rpkt);
    }
}
