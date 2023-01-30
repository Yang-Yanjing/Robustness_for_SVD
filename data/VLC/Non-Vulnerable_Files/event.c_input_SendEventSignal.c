}
void input_SendEventSignal( input_thread_t *p_input, double f_quality, double f_strength )
{
    vlc_value_t val;
    val.f_float = f_quality;
    var_Change( p_input, "signal-quality", VLC_VAR_SETVALUE, &val, NULL );
    val.f_float = f_strength;
    var_Change( p_input, "signal-strength", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_SIGNAL );
}
