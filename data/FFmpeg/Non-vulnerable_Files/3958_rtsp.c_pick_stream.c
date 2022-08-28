static int pick_stream(AVFormatContext *s, RTSPStream **rtsp_st,
                       const uint8_t *buf, int len)
{
    RTSPState *rt = s->priv_data;
    int i;
    if (len < 0)
        return len;
    if (rt->nb_rtsp_streams == 1) {
        *rtsp_st = rt->rtsp_streams[0];
        return len;
    }
    if (len >= 8 && rt->transport == RTSP_TRANSPORT_RTP) {
        if (RTP_PT_IS_RTCP(rt->recvbuf[1])) {
            int no_ssrc = 0;
            for (i = 0; i < rt->nb_rtsp_streams; i++) {
                RTPDemuxContext *rtpctx = rt->rtsp_streams[i]->transport_priv;
                if (!rtpctx)
                    continue;
                if (rtpctx->ssrc == AV_RB32(&buf[4])) {
                    *rtsp_st = rt->rtsp_streams[i];
                    return len;
                }
                if (!rtpctx->ssrc)
                    no_ssrc = 1;
            }
            if (no_ssrc) {
                av_log(s, AV_LOG_WARNING,
                       "Unable to pick stream for packet - SSRC not known for "
                       "all streams\n");
                return AVERROR(EAGAIN);
            }
        } else {
            for (i = 0; i < rt->nb_rtsp_streams; i++) {
                if ((buf[1] & 0x7f) == rt->rtsp_streams[i]->sdp_payload_type) {
                    *rtsp_st = rt->rtsp_streams[i];
                    return len;
                }
            }
        }
    }
    av_log(s, AV_LOG_WARNING, "Unable to pick stream for packet\n");
    return AVERROR(EAGAIN);
}
