}
void input_SendEventTeletextAdd( input_thread_t *p_input,
                                 int i_teletext, const char *psz_text )
{
    VarListAdd( p_input, "teletext-es", INPUT_EVENT_TELETEXT, i_teletext, psz_text );
}
