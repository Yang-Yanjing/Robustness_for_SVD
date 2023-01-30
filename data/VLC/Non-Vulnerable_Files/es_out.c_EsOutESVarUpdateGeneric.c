}
static void EsOutESVarUpdateGeneric( es_out_t *out, int i_id,
                                     const es_format_t *fmt, const char *psz_language,
                                     bool b_delete )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    vlc_value_t       val, text;
    if( b_delete )
    {
        if( EsFmtIsTeletext( fmt ) )
            input_SendEventTeletextDel( p_sys->p_input, i_id );
        input_SendEventEsDel( p_input, fmt->i_cat, i_id );
        return;
    }
    /* Get the number of ES already added */
    const char *psz_var;
    if( fmt->i_cat == AUDIO_ES )
        psz_var = "audio-es";
    else if( fmt->i_cat == VIDEO_ES )
        psz_var = "video-es";
    else
        psz_var = "spu-es";
    var_Change( p_input, psz_var, VLC_VAR_CHOICESCOUNT, &val, NULL );
    if( val.i_int == 0 )
    {
        vlc_value_t val2;
        /* First one, we need to add the "Disable" choice */
        val2.i_int = -1; text.psz_string = _("Disable");
        var_Change( p_input, psz_var, VLC_VAR_ADDCHOICE, &val2, &text );
        val.i_int++;
    }
    /* Take care of the ES description */
    if( fmt->psz_description && *fmt->psz_description )
    {
        if( psz_language && *psz_language )
        {
            if( asprintf( &text.psz_string, "%s - [%s]", fmt->psz_description,
                          psz_language ) == -1 )
                text.psz_string = NULL;
        }
        else text.psz_string = strdup( fmt->psz_description );
    }
    else
    {
        if( psz_language && *psz_language )
        {
            if( asprintf( &text.psz_string, "%s %"PRId64" - [%s]", _( "Track" ), val.i_int, psz_language ) == -1 )
                text.psz_string = NULL;
        }
        else
        {
            if( asprintf( &text.psz_string, "%s %"PRId64, _( "Track" ), val.i_int ) == -1 )
                text.psz_string = NULL;
        }
    }
    input_SendEventEsAdd( p_input, fmt->i_cat, i_id, text.psz_string );
    if( EsFmtIsTeletext( fmt ) )
    {
        char psz_page[3+1];
        snprintf( psz_page, sizeof(psz_page), "%d%2.2x",
                  fmt->subs.teletext.i_magazine,
                  fmt->subs.teletext.i_page );
        input_SendEventTeletextAdd( p_sys->p_input,
                                    i_id, fmt->subs.teletext.i_magazine >= 0 ? psz_page : NULL );
    }
    free( text.psz_string );
}
