}
void input_SendEventSubtitleDelay( input_thread_t *p_input, mtime_t i_delay )
{
    vlc_value_t val;
    val.i_time = i_delay;
    var_Change( p_input, "spu-delay", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_SUBTITLE_DELAY );
}
