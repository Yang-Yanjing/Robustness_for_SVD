/* TODO and file name ? */
void input_SendEventRecord( input_thread_t *p_input, bool b_recording )
{
    vlc_value_t val;
    val.b_bool = b_recording;
    var_Change( p_input, "record", VLC_VAR_SETVALUE, &val, NULL );
    Trigger( p_input, INPUT_EVENT_RECORD );
}
