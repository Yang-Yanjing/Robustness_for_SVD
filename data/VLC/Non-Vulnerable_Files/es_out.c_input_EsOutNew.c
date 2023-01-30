 *****************************************************************************/
es_out_t *input_EsOutNew( input_thread_t *p_input, int i_rate )
{
    es_out_t     *out = malloc( sizeof( *out ) );
    if( !out )
        return NULL;
    es_out_sys_t *p_sys = calloc( 1, sizeof( *p_sys ) );
    if( !p_sys )
    {
        free( out );
        return NULL;
    }
    out->pf_add     = EsOutAdd;
    out->pf_send    = EsOutSend;
    out->pf_del     = EsOutDel;
    out->pf_control = EsOutControl;
    out->pf_destroy = EsOutDelete;
    out->p_sys      = p_sys;
    vlc_mutex_init_recursive( &p_sys->lock );
    p_sys->p_input = p_input;
    p_sys->b_active = false;
    p_sys->i_mode   = ES_OUT_MODE_NONE;
    TAB_INIT( p_sys->i_pgrm, p_sys->pgrm );
    TAB_INIT( p_sys->i_es, p_sys->es );
    /* */
    p_sys->i_group_id = var_GetInteger( p_input, "program" );
    p_sys->i_audio_last = var_GetInteger( p_input, "audio-track" );
    p_sys->i_sub_last = var_GetInteger( p_input, "sub-track" );
    p_sys->i_default_sub_id   = -1;
    if( !p_input->b_preparsing )
    {
        char *psz_string;
        psz_string = var_GetString( p_input, "audio-language" );
        p_sys->ppsz_audio_language = LanguageSplit( psz_string, true );
        if( p_sys->ppsz_audio_language )
        {
            for( int i = 0; p_sys->ppsz_audio_language[i]; i++ )
                msg_Dbg( p_input, "selected audio language[%d] %s",
                         i, p_sys->ppsz_audio_language[i] );
        }
        free( psz_string );
        psz_string = var_GetString( p_input, "sub-language" );
        p_sys->ppsz_sub_language = LanguageSplit( psz_string, false );
        if( p_sys->ppsz_sub_language )
        {
            for( int i = 0; p_sys->ppsz_sub_language[i]; i++ )
                msg_Dbg( p_input, "selected subtitle language[%d] %s",
                         i, p_sys->ppsz_sub_language[i] );
        }
        free( psz_string );
    }
    p_sys->i_audio_id = var_GetInteger( p_input, "audio-track-id" );
    p_sys->i_sub_id = var_GetInteger( p_input, "sub-track-id" );
    p_sys->i_pause_date = -1;
    p_sys->i_rate = i_rate;
    p_sys->b_buffering = true;
    p_sys->i_preroll_end = -1;
    return out;
}
