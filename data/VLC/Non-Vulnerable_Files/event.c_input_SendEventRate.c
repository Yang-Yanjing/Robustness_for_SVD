}
void input_SendEventRate( input_thread_t *p_input, int i_rate )
{
    vlc_value_t val;
    val.f_float = (float)INPUT_RATE_DEFAULT / (float)i_rate;
    var_Change( p_input, "rate", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_RATE );
}
