/** Check MIT-SHM shared memory support */
bool XCB_shm_Check (vlc_object_t *obj, xcb_connection_t *conn)
{
#ifdef HAVE_SYS_SHM_H
    xcb_shm_query_version_cookie_t ck;
    xcb_shm_query_version_reply_t *r;
    ck = xcb_shm_query_version (conn);
    r = xcb_shm_query_version_reply (conn, ck, NULL);
    if (r != NULL)
    {
        free (r);
        return true;
    }
    msg_Err (obj, "shared memory (MIT-SHM) not available");
    msg_Warn (obj, "display will be slow");
#else
    msg_Warn (obj, "shared memory (MIT-SHM) not implemented");
    (void) conn;
#endif
    return false;
}
