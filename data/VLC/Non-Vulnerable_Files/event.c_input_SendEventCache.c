}
void input_SendEventCache( input_thread_t *p_input, double f_level )
{
    vlc_value_t val;
    val.f_float = f_level;
    var_Change( p_input, "cache", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_CACHE );
}
