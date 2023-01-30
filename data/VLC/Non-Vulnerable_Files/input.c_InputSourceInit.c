 *****************************************************************************/
static int InputSourceInit( input_thread_t *p_input,
                            input_source_t *in, const char *psz_mrl,
                            const char *psz_forced_demux, bool b_in_can_fail )
{
    const char *psz_access, *psz_demux, *psz_path, *psz_anchor = NULL;
    char *psz_var_demux = NULL;
    double f_fps;
    assert( psz_mrl );
    char *psz_dup = strdup( psz_mrl );
    if( psz_dup == NULL )
        goto error;
    /* Split uri */
    input_SplitMRL( &psz_access, &psz_demux, &psz_path, &psz_anchor, psz_dup );
    msg_Dbg( p_input, "`%s' gives access `%s' demux `%s' path `%s'",
             psz_mrl, psz_access, psz_demux, psz_path );
    if( !p_input->b_preparsing )
    {
        /* Find optional titles and seekpoints */
        MRLSections( psz_anchor, &in->i_title_start, &in->i_title_end,
                     &in->i_seekpoint_start, &in->i_seekpoint_end );
        if( psz_forced_demux && *psz_forced_demux )
        {
            psz_demux = psz_forced_demux;
        }
        else if( *psz_demux == '\0' )
        {
            /* special hack for forcing a demuxer with --demux=module
             * (and do nothing with a list) */
            psz_var_demux = var_GetNonEmptyString( p_input, "demux" );
            psz_demux = (psz_var_demux != NULL) ? psz_var_demux : "any";
            msg_Dbg( p_input, "specified demux `%s'", psz_demux );
        }
        /* Try access_demux first */
        in->p_demux = demux_New( p_input, p_input, psz_access, psz_demux,
                                 psz_path, NULL, p_input->p->p_es_out, false );
    }
    else
    {
        /* Preparsing is only for file:// */
        if( *psz_demux )
            goto error;
        if( strcmp( psz_access, "file" ) )
            goto error;
        msg_Dbg( p_input, "trying to pre-parse %s",  psz_path );
    }
    mtime_t i_pts_delay;
    if( in->p_demux )
    {
        /* Get infos from access_demux */
        in->b_title_demux = true;
        if( demux_Control( in->p_demux, DEMUX_GET_TITLE_INFO,
                            &in->title, &in->i_title,
                            &in->i_title_offset, &in->i_seekpoint_offset ) )
        {
            TAB_INIT( in->i_title, in->title );
        }
        if( demux_Control( in->p_demux, DEMUX_CAN_CONTROL_PACE,
                            &in->b_can_pace_control ) )
            in->b_can_pace_control = false;
        assert( in->p_demux->pf_demux != NULL || !in->b_can_pace_control );
        if( !in->b_can_pace_control )
        {
            if( demux_Control( in->p_demux, DEMUX_CAN_CONTROL_RATE,
                                &in->b_can_rate_control, &in->b_rescale_ts ) )
            {
                in->b_can_rate_control = false;
                in->b_rescale_ts = true; /* not used */
            }
        }
        else
        {
            in->b_can_rate_control = true;
            in->b_rescale_ts = true;
        }
        if( demux_Control( in->p_demux, DEMUX_CAN_PAUSE,
                            &in->b_can_pause ) )
            in->b_can_pause = false;
        var_SetBool( p_input, "can-pause", in->b_can_pause || !in->b_can_pace_control ); /* XXX temporary because of es_out_timeshift*/
        var_SetBool( p_input, "can-rate", !in->b_can_pace_control || in->b_can_rate_control ); /* XXX temporary because of es_out_timeshift*/
        var_SetBool( p_input, "can-rewind", !in->b_rescale_ts && !in->b_can_pace_control && in->b_can_rate_control );
        bool b_can_seek;
        if( demux_Control( in->p_demux, DEMUX_CAN_SEEK, &b_can_seek ) )
            b_can_seek = false;
        var_SetBool( p_input, "can-seek", b_can_seek );
    }
    else
    {   /* Now try a real access */
        access_t *p_access = access_New( p_input, p_input,
                                         psz_access, psz_demux, psz_path );
        if( p_access == NULL )
        {
            if( vlc_object_alive( p_input ) )
            {
                msg_Err( p_input, "open of `%s' failed", psz_mrl );
                if( !b_in_can_fail )
                    dialog_Fatal( p_input, _("Your input can't be opened"),
                                   _("VLC is unable to open the MRL '%s'."
                                     " Check the log for details."), psz_mrl );
            }
            goto error;
        }
        /* Access-forced demuxer (PARENTAL ADVISORY: EXPLICIT HACK) */
        if( !psz_demux[0] || !strcasecmp( psz_demux, "any" ) )
            psz_demux = p_access->psz_demux;
        /* */
        int  i_input_list;
        char **ppsz_input_list;
        TAB_INIT( i_input_list, ppsz_input_list );
        /* On master stream only, use input-list */
        if( &p_input->p->input == in )
        {
            char *psz_list;
            char *psz_parser;
            psz_list =
            psz_parser = var_CreateGetNonEmptyString( p_input, "input-list" );
            while( psz_parser && *psz_parser )
            {
                char *p = strchr( psz_parser, ',' );
                if( p )
                    *p++ = '\0';
                if( *psz_parser )
                {
                    char *psz_name = strdup( psz_parser );
                    if( psz_name )
                        TAB_APPEND( i_input_list, ppsz_input_list, psz_name );
                }
                psz_parser = p;
            }
            free( psz_list );
        }
        /* Autodetect extra files if none specified */
        if( i_input_list <= 0 )
        {
            InputGetExtraFiles( p_input, &i_input_list, &ppsz_input_list,
                                psz_access, psz_path );
        }
        if( i_input_list > 0 )
            TAB_APPEND( i_input_list, ppsz_input_list, NULL );
        /* Create the stream_t */
        stream_t *p_stream = stream_AccessNew( p_access, ppsz_input_list );
        if( ppsz_input_list )
        {
            for( int i = 0; ppsz_input_list[i] != NULL; i++ )
                free( ppsz_input_list[i] );
            TAB_CLEAN( i_input_list, ppsz_input_list );
        }
        if( p_stream == NULL )
        {
            msg_Warn( p_input, "cannot create a stream_t from access" );
            goto error;
        }
        /* Add stream filters */
        char *psz_stream_filter = var_GetNonEmptyString( p_input,
                                                         "stream-filter" );
        p_stream = stream_FilterChainNew( p_stream, psz_stream_filter,
                               var_GetBool( p_input, "input-record-native" ) );
        free( psz_stream_filter );
        if( !p_input->b_preparsing )
        {
            bool b;
            stream_Control( p_stream, STREAM_CAN_CONTROL_PACE,
                            &in->b_can_pace_control );
            in->b_can_rate_control = in->b_can_pace_control;
            in->b_rescale_ts = true;
            stream_Control( p_stream, STREAM_CAN_PAUSE, &in->b_can_pause );
            var_SetBool( p_input, "can-pause",
                         in->b_can_pause || !in->b_can_pace_control ); /* XXX temporary because of es_out_timeshift*/
            var_SetBool( p_input, "can-rate",
                         !in->b_can_pace_control || in->b_can_rate_control ); /* XXX temporary because of es_out_timeshift*/
            var_SetBool( p_input, "can-rewind",
                         !in->b_rescale_ts && !in->b_can_pace_control );
            stream_Control( p_stream, STREAM_CAN_SEEK, &b );
            var_SetBool( p_input, "can-seek", b );
            in->b_title_demux = false;
            stream_Control( p_stream, STREAM_GET_PTS_DELAY, &i_pts_delay );
        }
        in->p_demux = demux_New( p_input, p_input, psz_access, psz_demux,
                   /* Take access/stream redirections into account: */
                            p_stream->psz_path ? p_stream->psz_path : psz_path,
                                 p_stream, p_input->p->p_es_out,
                                 p_input->b_preparsing );
        if( in->p_demux == NULL )
        {
            if( vlc_object_alive( p_input ) )
            {
                msg_Err( p_input, "no suitable demux module for `%s/%s://%s'",
                         psz_access, psz_demux, psz_path );
                if( !b_in_can_fail )
                    dialog_Fatal( VLC_OBJECT( p_input ),
                                  _("VLC can't recognize the input's format"),
                                  _("The format of '%s' cannot be detected. "
                                    "Have a look at the log for details."), psz_mrl );
            }
            stream_Delete( p_stream );
            goto error;
        }
        assert( in->p_demux->pf_demux != NULL );
        /* Get title from demux */
        if( !p_input->b_preparsing && in->i_title <= 0 )
        {
            if( demux_Control( in->p_demux, DEMUX_GET_TITLE_INFO,
                                &in->title, &in->i_title,
                                &in->i_title_offset, &in->i_seekpoint_offset ))
            {
                TAB_INIT( in->i_title, in->title );
            }
            else
            {
                in->b_title_demux = true;
            }
        }
    }
    free( psz_var_demux );
    free( psz_dup );
    /* Set record capabilities */
    if( demux_Control( in->p_demux, DEMUX_CAN_RECORD, &in->b_can_stream_record ) )
        in->b_can_stream_record = false;
#ifdef ENABLE_SOUT
    if( !var_GetBool( p_input, "input-record-native" ) )
        in->b_can_stream_record = false;
    var_SetBool( p_input, "can-record", true );
#else
    var_SetBool( p_input, "can-record", in->b_can_stream_record );
#endif
    /* get attachment
     * FIXME improve for b_preparsing: move it after GET_META and check psz_arturl */
    if( !p_input->b_preparsing )
    {
        int i_attachment;
        input_attachment_t **attachment;
        if( !demux_Control( in->p_demux, DEMUX_GET_ATTACHMENTS,
                             &attachment, &i_attachment ) )
        {
            vlc_mutex_lock( &p_input->p->p_item->lock );
            AppendAttachment( &p_input->p->i_attachment, &p_input->p->attachment, &p_input->p->attachment_demux,
                              i_attachment, attachment, in->p_demux );
            vlc_mutex_unlock( &p_input->p->p_item->lock );
        }
        /* PTS delay: request from demux first. This is required for
         * access_demux and some special cases like SDP demux. Otherwise,
         * fallback to access */
        if( demux_Control( in->p_demux, DEMUX_GET_PTS_DELAY,
                           &in->i_pts_delay ) )
            in->i_pts_delay = i_pts_delay;
        if( in->i_pts_delay > INPUT_PTS_DELAY_MAX )
            in->i_pts_delay = INPUT_PTS_DELAY_MAX;
        else if( in->i_pts_delay < 0 )
            in->i_pts_delay = 0;
    }
    if( !demux_Control( in->p_demux, DEMUX_GET_FPS, &f_fps ) && f_fps > 0.0 )
    {
        vlc_mutex_lock( &p_input->p->p_item->lock );
        p_input->p->f_fps = f_fps;
        vlc_mutex_unlock( &p_input->p->p_item->lock );
    }
    if( var_GetInteger( p_input, "clock-synchro" ) != -1 )
        in->b_can_pace_control = !var_GetInteger( p_input, "clock-synchro" );
    return VLC_SUCCESS;
error:
    if( in->p_demux )
        demux_Delete( in->p_demux );
    free( psz_var_demux );
    free( psz_dup );
    return VLC_EGENERIC;
}
