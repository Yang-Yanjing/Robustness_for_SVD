}
static void CloseServer (vlc_tls_creds_t *crd) {
    msg_Dbg(crd, "close secure transport server");
    vlc_tls_creds_sys_t *sys = crd->sys;
    if (sys->server_cert_chain)
        CFRelease(sys->server_cert_chain);
    free(sys);
}
