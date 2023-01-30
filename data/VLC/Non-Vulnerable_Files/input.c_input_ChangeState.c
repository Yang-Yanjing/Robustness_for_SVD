/* */
static void input_ChangeState( input_thread_t *p_input, int i_state )
{
    const bool b_changed = p_input->p->i_state != i_state;
    p_input->p->i_state = i_state;
    if( i_state == ERROR_S )
        p_input->b_error = true;
    else if( i_state == END_S )
        p_input->b_eof = true;
    if( b_changed )
    {
        input_item_SetErrorWhenReading( p_input->p->p_item, p_input->b_error );
        input_SendEventState( p_input, i_state );
    }
}
