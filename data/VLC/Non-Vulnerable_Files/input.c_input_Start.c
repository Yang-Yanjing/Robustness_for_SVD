 */
int input_Start( input_thread_t *p_input )
{
    /* Create thread and wait for its readiness. */
    p_input->p->is_running = !vlc_clone( &p_input->p->thread,
                                         Run, p_input, VLC_THREAD_PRIORITY_INPUT );
    if( !p_input->p->is_running )
    {
        input_ChangeState( p_input, ERROR_S );
        msg_Err( p_input, "cannot create input thread" );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
