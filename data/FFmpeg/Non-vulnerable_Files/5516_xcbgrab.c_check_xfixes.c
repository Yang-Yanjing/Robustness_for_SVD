static int check_xfixes(xcb_connection_t *conn)
{
    xcb_xfixes_query_version_cookie_t cookie;
    xcb_xfixes_query_version_reply_t *reply;
    cookie = xcb_xfixes_query_version(conn, XCB_XFIXES_MAJOR_VERSION,
                                      XCB_XFIXES_MINOR_VERSION);
    reply  = xcb_xfixes_query_version_reply(conn, cookie, NULL);
    if (reply) {
        free(reply);
        return 1;
    }
    return 0;
}
