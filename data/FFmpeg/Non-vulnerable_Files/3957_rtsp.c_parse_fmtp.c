static void parse_fmtp(AVFormatContext *s, RTSPState *rt,
                       int payload_type, const char *line)
{
    int i;
    for (i = 0; i < rt->nb_rtsp_streams; i++) {
        RTSPStream *rtsp_st = rt->rtsp_streams[i];
        if (rtsp_st->sdp_payload_type == payload_type &&
            rtsp_st->dynamic_handler &&
            rtsp_st->dynamic_handler->parse_sdp_a_line) {
            rtsp_st->dynamic_handler->parse_sdp_a_line(s, i,
                rtsp_st->dynamic_protocol_context, line);
        }
    }
}
