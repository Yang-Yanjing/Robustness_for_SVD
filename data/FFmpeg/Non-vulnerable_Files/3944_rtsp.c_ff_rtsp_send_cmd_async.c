int ff_rtsp_send_cmd_async(AVFormatContext *s, const char *method,
                           const char *url, const char *headers)
{
    return rtsp_send_cmd_with_content_async(s, method, url, headers, NULL, 0);
}
