}
void input_SendEventTeletextDel( input_thread_t *p_input, int i_teletext )
{
    VarListDel( p_input, "teletext-es", INPUT_EVENT_TELETEXT, i_teletext );
}
