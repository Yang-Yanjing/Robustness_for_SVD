VLC_NORETURN
static void *VoutDisplayEventKeyDispatch(void *data)
{
    vout_display_owner_sys_t *osys = data;
    for (;;) {
        block_t *event = block_FifoGet(osys->event.fifo);
        int cancel = vlc_savecancel();
        int key;
        memcpy(&key, event->p_buffer, sizeof(key));
        vout_SendEventKey(osys->vout, key);
        block_Release(event);
        vlc_restorecancel(cancel);
    }
}
