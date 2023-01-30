}
httpd_handler_t *httpd_HandlerNew (httpd_host_t *host, const char *url,
                                   const char *login, const char *password,
                                   httpd_handler_callback_t cb,
                                   httpd_handler_sys_t *data)
{
    (void) host; (void) url;
    (void) login; (void) password;
    (void) cb; (void) data;
    assert (0);
}
