}
static int ControlLockedGetWakeup( es_out_t *p_out, mtime_t *pi_wakeup )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( p_sys->b_delayed )
    {
        assert( !p_sys->p_input->p->b_can_pace_control );
        *pi_wakeup = 0;
    }
    else
    {
        *pi_wakeup = es_out_GetWakeup( p_sys->p_out );
    }
    return VLC_SUCCESS;
}
