 *****************************************************************************/
void input_SendEventProgramAdd( input_thread_t *p_input,
                                int i_program, const char *psz_text )
{
    VarListAdd( p_input, "program", INPUT_EVENT_PROGRAM, i_program, psz_text );
}
