}
void vout_DeleteDisplay(vout_display_t *vd, vout_display_state_t *state)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (state) {
        if (!osys->is_wrapper )
            state->cfg = osys->cfg;
        state->wm_state = osys->wm_state;
        state->sar.num  = osys->sar_initial.num;
        state->sar.den  = osys->sar_initial.den;
    }
    VoutDisplayDestroyRender(vd);
    if (osys->is_wrapper)
        SplitterClose(vd);
    vout_display_Delete(vd);
    if (osys->event.fifo) {
        vlc_cancel(osys->event.thread);
        vlc_join(osys->event.thread, NULL);
        block_FifoRelease(osys->event.fifo);
    }
    vlc_mutex_destroy(&osys->lock);
    free(osys);
}
