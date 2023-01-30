 *****************************************************************************/
void input_ControlPush( input_thread_t *p_input,
                        int i_type, vlc_value_t *p_val )
{
    vlc_mutex_lock( &p_input->p->lock_control );
    if( i_type == INPUT_CONTROL_SET_DIE )
    {
        /* Special case, empty the control */
        for( int i = 0; i < p_input->p->i_control; i++ )
        {
            input_control_t *p_ctrl = &p_input->p->control[i];
            ControlRelease( p_ctrl->i_type, p_ctrl->val );
        }
        p_input->p->i_control = 0;
    }
    if( p_input->p->i_control >= INPUT_CONTROL_FIFO_SIZE )
    {
        msg_Err( p_input, "input control fifo overflow, trashing type=%d",
                 i_type );
        if( p_val )
            ControlRelease( i_type, *p_val );
    }
    else
    {
        input_control_t c;
        c.i_type = i_type;
        if( p_val )
            c.val = *p_val;
        else
            memset( &c.val, 0, sizeof(c.val) );
        p_input->p->control[p_input->p->i_control++] = c;
    }
    vlc_cond_signal( &p_input->p->wait_control );
    vlc_mutex_unlock( &p_input->p->lock_control );
}
