static int handle_invoke_error(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    const uint8_t *data_end = pkt->data + pkt->size;
    char *tracked_method = NULL;
    int level = AV_LOG_ERROR;
    uint8_t tmpstr[256];
    int ret;
    if ((ret = find_tracked_method(s, pkt, 9, &tracked_method)) < 0)
        return ret;
    if (!ff_amf_get_field_value(pkt->data + 9, data_end,
                                "description", tmpstr, sizeof(tmpstr))) {
        if (tracked_method && (!strcmp(tracked_method, "_checkbw")      ||
                               !strcmp(tracked_method, "releaseStream") ||
                               !strcmp(tracked_method, "FCSubscribe")   ||
                               !strcmp(tracked_method, "FCPublish"))) {
            
            level = AV_LOG_WARNING;
            ret = 0;
        } else if (tracked_method && !strcmp(tracked_method, "getStreamLength")) {
            level = rt->live ? AV_LOG_DEBUG : AV_LOG_WARNING;
            ret = 0;
        } else if (tracked_method && !strcmp(tracked_method, "connect")) {
            ret = handle_connect_error(s, tmpstr);
            if (!ret) {
                rt->do_reconnect = 1;
                level = AV_LOG_VERBOSE;
            }
        } else
            ret = AVERROR_UNKNOWN;
        av_log(s, level, "Server error: %s\n", tmpstr);
    }
    av_free(tracked_method);
    return ret;
}
