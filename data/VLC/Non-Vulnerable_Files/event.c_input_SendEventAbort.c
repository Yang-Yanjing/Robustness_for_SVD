}
void input_SendEventAbort( input_thread_t *p_input )
{
    Trigger( p_input, INPUT_EVENT_ABORT );
}
