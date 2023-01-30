}
static void VarListAdd( input_thread_t *p_input,
                        const char *psz_variable, int i_event,
                        int i_value, const char *psz_text )
{
    vlc_value_t val;
    vlc_value_t text;
    val.i_int = i_value;
    text.psz_string = (char*)psz_text;
    var_Change( p_input, psz_variable, VLC_VAR_ADDCHOICE,
                &val, psz_text ? &text : NULL );
    Trigger( p_input, i_event );
}
