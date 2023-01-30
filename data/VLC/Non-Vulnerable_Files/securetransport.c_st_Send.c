 */
static int st_Send (void *opaque, const void *buf, size_t length)
{
    vlc_tls_t *session = opaque;
    vlc_tls_sys_t *sys = session->sys;
    OSStatus ret = noErr;
    /*
     * SSLWrite does not return the number of bytes actually written to
     * the socket, but the number of bytes written to the internal cache.
     *
     * If return value is errSSLWouldBlock, the underlying socket cannot
     * send all data, but the data is already cached. In this situation,
     * we need to call SSLWrite again. To ensure this call even for the
     * last bytes, we return EAGAIN. On the next call, we give no new data
     * to SSLWrite until the error is not errSSLWouldBlock anymore.
     *
     * This code is adapted the same way as done in curl.
     * (https://github.com/bagder/curl/blob/master/lib/curl_darwinssl.c#L2067)
     */
    /* EAGAIN is not expected by net_Write in this situation,
       so use EINTR here */
    int againErr = sys->b_server_mode ? EAGAIN : EINTR;
    size_t actualSize;
    if (sys->i_send_buffered_bytes > 0) {
        ret = SSLWrite(sys->p_context, NULL, 0, &actualSize);
        if (ret == noErr) {
            /* actualSize remains zero because no new data send */
            actualSize = sys->i_send_buffered_bytes;
            sys->i_send_buffered_bytes = 0;
        } else if (ret == errSSLWouldBlock) {
            errno = againErr;
            return -1;
        }
    } else {
        ret = SSLWrite(sys->p_context, buf, length, &actualSize);
        if (ret == errSSLWouldBlock) {
            sys->i_send_buffered_bytes = length;
            errno = againErr;
            return -1;
        }
    }
    return ret != noErr ? st_Error(session, ret) : actualSize;
}
