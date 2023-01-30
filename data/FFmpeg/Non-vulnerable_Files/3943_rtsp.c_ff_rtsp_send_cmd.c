int ff_rtsp_send_cmd(AVFormatContext *s, const char *method, const char *url,
                     const char *headers, RTSPMessageHeader *reply,
                     unsigned char **content_ptr)
{
    return ff_rtsp_send_cmd_with_content(s, method, url, headers, reply,
                                         content_ptr, NULL, 0);
}
