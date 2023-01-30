}
static mtime_t EsOutGetWakeup( es_out_t *out )
{
    es_out_sys_t   *p_sys = out->p_sys;
    input_thread_t *p_input = p_sys->p_input;
    if( !p_sys->p_pgrm )
        return 0;
    /* We do not have a wake up date if the input cannot have its speed
     * controlled or sout is imposing its own or while buffering
     *
     * FIXME for !p_input->p->b_can_pace_control a wake-up time is still needed
     * to avoid too heavy buffering */
    if( !p_input->p->b_can_pace_control ||
        p_input->p->b_out_pace_control ||
        p_sys->b_buffering )
        return 0;
    return input_clock_GetWakeup( p_sys->p_pgrm->p_clock );
}
