void ff_rtsp_close_connections(AVFormatContext *s)
{
    RTSPState *rt = s->priv_data;
    if (rt->rtsp_hd_out != rt->rtsp_hd) ffurl_close(rt->rtsp_hd_out);
    ffurl_close(rt->rtsp_hd);
    rt->rtsp_hd = rt->rtsp_hd_out = NULL;
}
