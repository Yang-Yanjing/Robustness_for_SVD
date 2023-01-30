}
void input_SendEventPosition( input_thread_t *p_input, double f_position, mtime_t i_time )
{
    vlc_value_t val;
    /* */
    val.f_float = f_position;
    var_Change( p_input, "position", VLC_VAR_SETVALUE, &val, NULL );
    /* */
    val.i_time = i_time;
    var_Change( p_input, "time", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_POSITION );
}
