 */
static int OpenClient (vlc_tls_creds_t *crd) {
    msg_Dbg(crd, "open st client");
    vlc_tls_creds_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->whitelist = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    sys->server_cert_chain = NULL;
    crd->sys = sys;
    crd->open = st_ClientSessionOpen;
    crd->close = st_SessionClose;
    return VLC_SUCCESS;
}
