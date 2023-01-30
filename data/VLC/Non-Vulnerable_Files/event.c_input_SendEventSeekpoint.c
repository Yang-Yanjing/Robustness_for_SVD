}
void input_SendEventSeekpoint( input_thread_t *p_input, int i_title, int i_seekpoint )
{
    vlc_value_t val;
    /* "chapter" */
    val.i_int = i_seekpoint;
    var_Change( p_input, "chapter", VLC_VAR_SETVALUE, &val, NULL );
    /* "title %2i" */
    char psz_title[10];
    snprintf( psz_title, sizeof(psz_title), "title %2i", i_title );
    var_Change( p_input, psz_title, VLC_VAR_SETVALUE, &val, NULL );
    /* */
    Trigger( p_input, INPUT_EVENT_CHAPTER );
}
