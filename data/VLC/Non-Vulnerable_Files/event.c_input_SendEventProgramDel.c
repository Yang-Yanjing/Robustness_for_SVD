}
void input_SendEventProgramDel( input_thread_t *p_input, int i_program )
{
    VarListDel( p_input, "program", INPUT_EVENT_PROGRAM, i_program );
}
