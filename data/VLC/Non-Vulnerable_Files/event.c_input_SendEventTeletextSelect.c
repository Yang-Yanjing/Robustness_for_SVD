}
void input_SendEventTeletextSelect( input_thread_t *p_input, int i_teletext )
{
    VarListSelect( p_input, "teletext-es", INPUT_EVENT_TELETEXT, i_teletext );
}
