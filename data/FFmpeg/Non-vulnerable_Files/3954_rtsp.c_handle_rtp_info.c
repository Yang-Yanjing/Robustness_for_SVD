static void handle_rtp_info(RTSPState *rt, const char *url,
                            uint32_t seq, uint32_t rtptime)
{
    int i;
    if (!rtptime || !url[0])
        return;
    if (rt->transport != RTSP_TRANSPORT_RTP)
        return;
    for (i = 0; i < rt->nb_rtsp_streams; i++) {
        RTSPStream *rtsp_st = rt->rtsp_streams[i];
        RTPDemuxContext *rtpctx = rtsp_st->transport_priv;
        if (!rtpctx)
            continue;
        if (!strcmp(rtsp_st->control_url, url)) {
            rtpctx->base_timestamp = rtptime;
            break;
        }
    }
}
