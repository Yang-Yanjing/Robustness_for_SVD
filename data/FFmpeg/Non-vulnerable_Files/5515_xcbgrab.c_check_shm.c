static int check_shm(xcb_connection_t *conn)
{
    xcb_shm_query_version_cookie_t cookie = xcb_shm_query_version(conn);
    xcb_shm_query_version_reply_t *reply;
    reply = xcb_shm_query_version_reply(conn, cookie, NULL);
    if (reply) {
        free(reply);
        return 1;
    }
    return 0;
}
