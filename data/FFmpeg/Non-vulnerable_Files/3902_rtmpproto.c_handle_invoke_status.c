static int handle_invoke_status(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    const uint8_t *data_end = pkt->data + pkt->size;
    const uint8_t *ptr = pkt->data + RTMP_HEADER;
    uint8_t tmpstr[256];
    int i, t;
    for (i = 0; i < 2; i++) {
        t = ff_amf_tag_size(ptr, data_end);
        if (t < 0)
            return 1;
        ptr += t;
    }
    t = ff_amf_get_field_value(ptr, data_end, "level", tmpstr, sizeof(tmpstr));
    if (!t && !strcmp(tmpstr, "error")) {
        t = ff_amf_get_field_value(ptr, data_end,
                                   "description", tmpstr, sizeof(tmpstr));
        if (t || !tmpstr[0])
            t = ff_amf_get_field_value(ptr, data_end, "code",
                                       tmpstr, sizeof(tmpstr));
        if (!t)
            av_log(s, AV_LOG_ERROR, "Server error: %s\n", tmpstr);
        return -1;
    }
    t = ff_amf_get_field_value(ptr, data_end, "code", tmpstr, sizeof(tmpstr));
    if (!t && !strcmp(tmpstr, "NetStream.Play.Start")) rt->state = STATE_PLAYING;
    if (!t && !strcmp(tmpstr, "NetStream.Play.Stop")) rt->state = STATE_STOPPED;
    if (!t && !strcmp(tmpstr, "NetStream.Play.UnpublishNotify")) rt->state = STATE_STOPPED;
    if (!t && !strcmp(tmpstr, "NetStream.Publish.Start")) rt->state = STATE_PUBLISHING;
    if (!t && !strcmp(tmpstr, "NetStream.Seek.Notify")) rt->state = STATE_PLAYING;
    return 0;
}
