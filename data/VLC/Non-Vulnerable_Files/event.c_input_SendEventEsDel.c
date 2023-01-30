}
void input_SendEventEsDel( input_thread_t *p_input, int i_cat, int i_id )
{
    if( i_cat != UNKNOWN_ES )
        VarListDel( p_input, GetEsVarName( i_cat ), INPUT_EVENT_ES, i_id );
}
