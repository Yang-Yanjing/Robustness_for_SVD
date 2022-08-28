static int sdp_read_close(AVFormatContext *s)
{
    ff_rtsp_close_streams(s);
    ff_network_close();
    return 0;
}
