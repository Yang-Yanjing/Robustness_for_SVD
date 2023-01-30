}
httpd_host_t *vlc_http_HostNew (vlc_object_t *obj)
{
    msg_Err (obj, "HTTP server not compiled-in!");
    return NULL;
}
