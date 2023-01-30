 */
static int st_Recv (void *opaque, void *buf, size_t length)
{
    vlc_tls_t *session = opaque;
    vlc_tls_sys_t *sys = session->sys;
    size_t actualSize;
    OSStatus ret = SSLRead(sys->p_context, buf, length, &actualSize);
    if (ret == errSSLWouldBlock && actualSize)
        return actualSize;
    /* peer performed shutdown */
    if (ret == errSSLClosedNoNotify || ret == errSSLClosedGraceful) {
        msg_Dbg(session, "Got close notification with code %d", ret);
        return 0;
    }
    return ret != noErr ? st_Error(session, ret) : actualSize;
}
