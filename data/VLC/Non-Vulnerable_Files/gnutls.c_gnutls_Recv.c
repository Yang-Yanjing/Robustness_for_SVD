 */
static int gnutls_Recv (void *opaque, void *buf, size_t length)
{
    vlc_tls_t *session = opaque;
    vlc_tls_sys_t *sys = session->sys;
    int val = gnutls_record_recv (sys->session, buf, length);
    return (val < 0) ? gnutls_Error (session, val) : val;
}
