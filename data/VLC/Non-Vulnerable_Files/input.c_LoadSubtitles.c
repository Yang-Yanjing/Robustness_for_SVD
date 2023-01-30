}
static void LoadSubtitles( input_thread_t *p_input )
{
    /* Load subtitles */
    /* Get fps and set it if not already set */
    const double f_fps = p_input->p->f_fps;
    if( f_fps > 1.0 )
    {
        float f_requested_fps;
        var_Create( p_input, "sub-original-fps", VLC_VAR_FLOAT );
        var_SetFloat( p_input, "sub-original-fps", f_fps );
        f_requested_fps = var_CreateGetFloat( p_input, "sub-fps" );
        if( f_requested_fps != f_fps )
        {
            var_Create( p_input, "sub-fps", VLC_VAR_FLOAT|
                                            VLC_VAR_DOINHERIT );
            var_SetFloat( p_input, "sub-fps", f_requested_fps );
        }
    }
    const int i_delay = var_CreateGetInteger( p_input, "sub-delay" );
    if( i_delay != 0 )
        var_SetTime( p_input, "spu-delay", (mtime_t)i_delay * 100000 );
    /* Look for and add subtitle files */
    unsigned i_flags = SUB_FORCED;
    char *psz_subtitle = var_GetNonEmptyString( p_input, "sub-file" );
    if( psz_subtitle != NULL )
    {
        msg_Dbg( p_input, "forced subtitle: %s", psz_subtitle );
        input_SubtitleFileAdd( p_input, psz_subtitle, i_flags );
        i_flags = SUB_NOFLAG;
    }
    if( var_GetBool( p_input, "sub-autodetect-file" ) )
    {
        char *psz_autopath = var_GetNonEmptyString( p_input, "sub-autodetect-path" );
        char **ppsz_subs = subtitles_Detect( p_input, psz_autopath,
                                             p_input->p->p_item->psz_uri );
        free( psz_autopath );
        for( int i = 0; ppsz_subs && ppsz_subs[i]; i++ )
        {
            if( !psz_subtitle || strcmp( psz_subtitle, ppsz_subs[i] ) )
            {
                i_flags |= SUB_CANFAIL;
                input_SubtitleFileAdd( p_input, ppsz_subs[i], i_flags );
                i_flags = SUB_NOFLAG;
            }
            free( ppsz_subs[i] );
        }
        free( ppsz_subs );
    }
    free( psz_subtitle );
    /* Load subtitles from attachments */
    int i_attachment = 0;
    input_attachment_t **pp_attachment = NULL;
    vlc_mutex_lock( &p_input->p->p_item->lock );
    for( int i = 0; i < p_input->p->i_attachment; i++ )
    {
        const input_attachment_t *a = p_input->p->attachment[i];
        if( !strcmp( a->psz_mime, "application/x-srt" ) )
            TAB_APPEND( i_attachment, pp_attachment,
                        vlc_input_attachment_New( a->psz_name, NULL,
                                                  a->psz_description, NULL, 0 ) );
    }
    vlc_mutex_unlock( &p_input->p->p_item->lock );
    if( i_attachment > 0 )
        var_Create( p_input, "sub-description", VLC_VAR_STRING );
    for( int i = 0; i < i_attachment; i++ )
    {
        input_attachment_t *a = pp_attachment[i];
        if( !a )
            continue;
        char *psz_mrl;
        if( a->psz_name[0] &&
            asprintf( &psz_mrl, "attachment://%s", a->psz_name ) >= 0 )
        {
            var_SetString( p_input, "sub-description", a->psz_description ? a->psz_description : "");
            input_SubtitleAdd( p_input, psz_mrl, i_flags );
            i_flags = SUB_NOFLAG;
            free( psz_mrl );
        }
        vlc_input_attachment_Delete( a );
    }
    free( pp_attachment );
    if( i_attachment > 0 )
        var_Destroy( p_input, "sub-description" );
}
