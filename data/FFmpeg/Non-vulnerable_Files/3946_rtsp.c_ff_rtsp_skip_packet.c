void ff_rtsp_skip_packet(AVFormatContext *s)
{
    RTSPState *rt = s->priv_data;
    int ret, len, len1;
    uint8_t buf[1024];
    ret = ffurl_read_complete(rt->rtsp_hd, buf, 3);
    if (ret != 3)
        return;
    len = AV_RB16(buf + 1);
    av_log(s, AV_LOG_TRACE, "skipping RTP packet len=%d\n", len);
    
    while (len > 0) {
        len1 = len;
        if (len1 > sizeof(buf))
            len1 = sizeof(buf);
        ret = ffurl_read_complete(rt->rtsp_hd, buf, len1);
        if (ret != len1)
            return;
        len -= len1;
    }
}
