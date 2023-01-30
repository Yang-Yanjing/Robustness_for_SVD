}
void input_SendEventEsAdd( input_thread_t *p_input, int i_cat, int i_id, const char *psz_text )
{
    if( i_cat != UNKNOWN_ES )
        VarListAdd( p_input, GetEsVarName( i_cat ), INPUT_EVENT_ES,
                    i_id, psz_text );
}
