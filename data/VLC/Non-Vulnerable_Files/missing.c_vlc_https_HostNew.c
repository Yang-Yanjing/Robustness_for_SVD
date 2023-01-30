}
httpd_host_t *vlc_https_HostNew (vlc_object_t *obj)
{
    msg_Err (obj, "HTTPS server not compiled-in!");
    return NULL;
}
