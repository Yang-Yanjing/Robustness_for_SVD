 *****************************************************************************/
void input_SendEventDead( input_thread_t *p_input )
{
    p_input->b_dead = true;
    Trigger( p_input, INPUT_EVENT_DEAD );
}
