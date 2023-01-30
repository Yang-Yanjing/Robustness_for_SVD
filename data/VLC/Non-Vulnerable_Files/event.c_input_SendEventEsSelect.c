/* i_id == -1 will unselect */
void input_SendEventEsSelect( input_thread_t *p_input, int i_cat, int i_id )
{
    if( i_cat != UNKNOWN_ES )
        VarListSelect( p_input, GetEsVarName( i_cat ), INPUT_EVENT_ES, i_id );
}
