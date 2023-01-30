}
static void VoutDisplayEventKey(vout_display_t *vd, int key)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (!osys->event.fifo) {
        osys->event.fifo = block_FifoNew();
        if (!osys->event.fifo)
            return;
        if (vlc_clone(&osys->event.thread, VoutDisplayEventKeyDispatch,
                      osys, VLC_THREAD_PRIORITY_LOW)) {
            block_FifoRelease(osys->event.fifo);
            osys->event.fifo = NULL;
            return;
        }
    }
    block_t *event = block_Alloc(sizeof(key));
    if (event) {
        memcpy(event->p_buffer, &key, sizeof(key));
        block_FifoPut(osys->event.fifo, event);
    }
}
