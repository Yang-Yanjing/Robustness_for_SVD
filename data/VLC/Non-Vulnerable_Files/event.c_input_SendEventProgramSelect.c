}
void input_SendEventProgramSelect( input_thread_t *p_input, int i_program )
{
    VarListSelect( p_input, "program", INPUT_EVENT_PROGRAM, i_program );
}
