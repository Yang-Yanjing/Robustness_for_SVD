}
httpd_file_t *httpd_FileNew (httpd_host_t *host,
                             const char *url, const char *content_type,
                             const char *login, const char *password,
                             httpd_file_callback_t cb, httpd_file_sys_t *data)
{
    (void) host;
    (void) url; (void) content_type;
    (void) login; (void) password;
    (void) cb; (void) data;
    assert (0);
}
