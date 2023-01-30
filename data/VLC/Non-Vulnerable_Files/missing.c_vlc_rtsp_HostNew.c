}
httpd_host_t *vlc_rtsp_HostNew (vlc_object_t *obj)
{
    msg_Err (obj, "RTSP server not compiled-in!");
    return NULL;
}
