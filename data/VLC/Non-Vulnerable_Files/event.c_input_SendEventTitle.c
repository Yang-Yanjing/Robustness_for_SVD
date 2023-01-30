}
void input_SendEventTitle( input_thread_t *p_input, int i_title )
{
    vlc_value_t val;
    val.i_int = i_title;
    var_Change( p_input, "title", VLC_VAR_SETVALUE, &val, NULL );
    input_ControlVarTitle( p_input, i_title );
    Trigger( p_input, INPUT_EVENT_TITLE );
}
