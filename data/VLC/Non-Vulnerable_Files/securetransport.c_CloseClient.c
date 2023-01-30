}
static void CloseClient (vlc_tls_creds_t *crd) {
    msg_Dbg(crd, "close secure transport client");
    vlc_tls_creds_sys_t *sys = crd->sys;
    if (sys->whitelist)
        CFRelease(sys->whitelist);
    free(sys);
}
