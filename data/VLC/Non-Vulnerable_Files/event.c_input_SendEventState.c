}
void input_SendEventState( input_thread_t *p_input, int i_state )
{
    vlc_value_t val;
    val.i_int = i_state;
    var_Change( p_input, "state", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_STATE );
}
