 */
static void Close(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Terminate the thread. */
    vlc_cancel(p_sys->thread);
    vlc_join(p_sys->thread, NULL);
    /* Free the ressources */
    vout_DeleteDisplay(p_sys->p_vd, NULL);
    vlc_object_release(p_sys->p_vout);
    block_FifoRelease(p_sys->fifo);
    free(p_sys->p_prev_s16_buff);
    vlc_sem_destroy(&p_sys->ready);
    free(p_sys);
}
