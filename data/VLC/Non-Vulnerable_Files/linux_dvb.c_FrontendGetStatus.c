}
void FrontendGetStatus( access_t *p_access, frontend_status_t *p_status )
{
    access_sys_t *p_sys = p_access->p_sys;
    frontend_t * p_frontend = p_sys->p_frontend;
    p_status->b_has_signal = (p_frontend->i_last_status & FE_HAS_SIGNAL) != 0;
    p_status->b_has_carrier = (p_frontend->i_last_status & FE_HAS_CARRIER) != 0;
    p_status->b_has_lock = (p_frontend->i_last_status & FE_HAS_LOCK) != 0;
}
