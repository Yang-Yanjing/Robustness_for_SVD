#ifdef ALLOW_DUMMY_VOUT
static void DummyVoutSendDisplayEventMouse(vout_thread_t *vout, vlc_mouse_t *fallback, const vlc_mouse_t *m)
{
    vout_thread_sys_t p;
    if (!vout->p) {
        p.mouse = *fallback;
        vlc_mutex_init(&p.filter.lock);
        p.filter.chain_static = NULL;
        p.filter.chain_interactive = NULL;
        p.spu = NULL;
        vout->p = &p;
    }
    vout_SendDisplayEventMouse(vout, m);
    if (vout->p == &p) {
        vlc_mutex_destroy(&p.filter.lock);
        *fallback = p.mouse;
        vout->p = NULL;
    }
}
