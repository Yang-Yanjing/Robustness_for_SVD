static void rtsp_reply_header(HTTPContext *c, enum RTSPStatusCode error_number)
{
    const char *str;
    time_t ti;
    struct tm *tm;
    char buf2[32];
    str = RTSP_STATUS_CODE2STRING(error_number);
    if (!str)
        str = "Unknown Error";
    avio_printf(c->pb, "RTSP/1.0 %d %s\r\n", error_number, str);
    avio_printf(c->pb, "CSeq: %d\r\n", c->seq);
    
    ti = time(NULL);
    tm = gmtime(&ti);
    strftime(buf2, sizeof(buf2), "%a, %d %b %Y %H:%M:%S", tm);
    avio_printf(c->pb, "Date: %s GMT\r\n", buf2);
}
