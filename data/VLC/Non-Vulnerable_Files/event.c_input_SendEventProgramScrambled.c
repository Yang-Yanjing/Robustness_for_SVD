}
void input_SendEventProgramScrambled( input_thread_t *p_input, int i_group, bool b_scrambled )
{
    if( var_GetInteger( p_input, "program" ) != i_group )
        return;
    var_SetBool( p_input, "program-scrambled", b_scrambled );
    Trigger( p_input, INPUT_EVENT_PROGRAM );
}
