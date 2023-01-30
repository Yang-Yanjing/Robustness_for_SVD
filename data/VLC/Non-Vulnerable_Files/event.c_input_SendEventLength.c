}
void input_SendEventLength( input_thread_t *p_input, mtime_t i_length )
{
    vlc_value_t val;
    /* FIXME ugly + what about meta change event ? */
    if( var_GetTime( p_input, "length" ) == i_length )
        return;
    input_item_SetDuration( p_input->p->p_item, i_length );
    val.i_time = i_length;
    var_Change( p_input, "length", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_LENGTH );
}
