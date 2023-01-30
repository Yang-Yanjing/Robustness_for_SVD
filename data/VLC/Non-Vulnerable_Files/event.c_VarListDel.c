}
static void VarListDel( input_thread_t *p_input,
                        const char *psz_variable, int i_event,
                        int i_value )
{
    vlc_value_t val;
    if( i_value >= 0 )
    {
        val.i_int = i_value;
        var_Change( p_input, psz_variable, VLC_VAR_DELCHOICE, &val, NULL );
    }
    else
    {
        var_Change( p_input, psz_variable, VLC_VAR_CLEARCHOICES, &val, NULL );
    }
    Trigger( p_input, i_event );
}
