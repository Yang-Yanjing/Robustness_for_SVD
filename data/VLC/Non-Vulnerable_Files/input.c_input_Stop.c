 */
void input_Stop( input_thread_t *p_input, bool b_abort )
{
    /* Set die for input and ALL of this childrens (even (grand-)grand-childrens)
     * It is needed here even if it is done in INPUT_CONTROL_SET_DIE handler to
     * unlock the control loop */
    ObjectKillChildrens( VLC_OBJECT(p_input) );
    vlc_mutex_lock( &p_input->p->lock_control );
    p_input->p->b_abort |= b_abort;
    vlc_mutex_unlock( &p_input->p->lock_control );
    input_ControlPush( p_input, INPUT_CONTROL_SET_DIE, NULL );
}
