 *****************************************************************************/
static void CloseGeneric( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if(p_sys->omx_handle) DeinitialiseComponent(p_dec, p_sys->omx_handle);
    DeinitOmxCore();
    DeinitOmxEventQueue(&p_sys->event_queue);
    vlc_mutex_destroy (&p_sys->in.fifo.lock);
    vlc_cond_destroy (&p_sys->in.fifo.wait);
    vlc_mutex_destroy (&p_sys->out.fifo.lock);
    vlc_cond_destroy (&p_sys->out.fifo.wait);
    free( p_sys );
}
