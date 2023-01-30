}
static int Init( input_thread_t * p_input )
{
    int i;
    for( i = 0; i < p_input->p->p_item->i_options; i++ )
    {
        if( !strncmp( p_input->p->p_item->ppsz_options[i], "meta-file", 9 ) )
        {
            msg_Dbg( p_input, "Input is a meta file: disabling unneeded options" );
            var_SetString( p_input, "sout", "" );
            var_SetBool( p_input, "sout-all", false );
            var_SetString( p_input, "input-slave", "" );
            var_SetInteger( p_input, "input-repeat", 0 );
            var_SetString( p_input, "sub-file", "" );
            var_SetBool( p_input, "sub-autodetect-file", false );
        }
    }
    InitStatistics( p_input );
#ifdef ENABLE_SOUT
    if( InitSout( p_input ) )
        goto error;
#endif
    /* Create es out */
    p_input->p->p_es_out = input_EsOutTimeshiftNew( p_input, p_input->p->p_es_out_display, p_input->p->i_rate );
    /* */
    input_ChangeState( p_input, OPENING_S );
    input_SendEventCache( p_input, 0.0 );
    /* */
    if( InputSourceInit( p_input, &p_input->p->input,
                         p_input->p->p_item->psz_uri, NULL, false ) )
    {
        goto error;
    }
    InitTitle( p_input );
    /* Load master infos */
    /* Init length */
    mtime_t i_length;
    if( demux_Control( p_input->p->input.p_demux, DEMUX_GET_LENGTH,
                         &i_length ) )
        i_length = 0;
    if( i_length <= 0 )
        i_length = input_item_GetDuration( p_input->p->p_item );
    input_SendEventLength( p_input, i_length );
    input_SendEventPosition( p_input, 0.0, 0 );
    if( !p_input->b_preparsing )
    {
        StartTitle( p_input );
        LoadSubtitles( p_input );
        LoadSlaves( p_input );
        InitPrograms( p_input );
        double f_rate = var_InheritFloat( p_input, "rate" );
        if( f_rate != 0.0 && f_rate != 1.0 )
        {
            vlc_value_t val = { .i_int = INPUT_RATE_DEFAULT / f_rate };
            input_ControlPush( p_input, INPUT_CONTROL_SET_RATE, &val );
        }
    }
    if( !p_input->b_preparsing && p_input->p->p_sout )
    {
        p_input->p->b_out_pace_control = (p_input->p->p_sout->i_out_pace_nocontrol > 0);
        if( p_input->p->b_can_pace_control && p_input->p->b_out_pace_control )
        {
            /* We don't want a high input priority here or we'll
             * end-up sucking up all the CPU time */
            vlc_set_priority( p_input->p->thread, VLC_THREAD_PRIORITY_LOW );
        }
        msg_Dbg( p_input, "starting in %s mode",
                 p_input->p->b_out_pace_control ? "async" : "sync" );
    }
    vlc_meta_t *p_meta = vlc_meta_New();
    if( p_meta != NULL )
    {
        /* Get meta data from users */
        InputMetaUser( p_input, p_meta );
        /* Get meta data from master input */
        InputSourceMeta( p_input, &p_input->p->input, p_meta );
        /* And from slave */
        for( int i = 0; i < p_input->p->i_slave; i++ )
            InputSourceMeta( p_input, p_input->p->slave[i], p_meta );
        es_out_ControlSetMeta( p_input->p->p_es_out, p_meta );
        vlc_meta_Delete( p_meta );
    }
    msg_Dbg( p_input, "`%s' successfully opened",
             p_input->p->p_item->psz_uri );
    /* initialization is complete */
    input_ChangeState( p_input, PLAYING_S );
    return VLC_SUCCESS;
error:
    input_ChangeState( p_input, ERROR_S );
    if( p_input->p->p_es_out )
        es_out_Delete( p_input->p->p_es_out );
    es_out_SetMode( p_input->p->p_es_out_display, ES_OUT_MODE_END );
    if( p_input->p->p_resource )
    {
        if( p_input->p->p_sout )
            input_resource_RequestSout( p_input->p->p_resource,
                                         p_input->p->p_sout, NULL );
        input_resource_SetInput( p_input->p->p_resource, NULL );
        if( p_input->p->p_resource_private )
            input_resource_Terminate( p_input->p->p_resource_private );
    }
    if( !p_input->b_preparsing && libvlc_stats( p_input ) )
    {
#define EXIT_COUNTER( c ) do { if( p_input->p->counters.p_##c ) \
                                   stats_CounterClean( p_input->p->counters.p_##c );\
                               p_input->p->counters.p_##c = NULL; } while(0)
        EXIT_COUNTER( read_bytes );
        EXIT_COUNTER( read_packets );
        EXIT_COUNTER( demux_read );
        EXIT_COUNTER( input_bitrate );
        EXIT_COUNTER( demux_bitrate );
        EXIT_COUNTER( demux_corrupted );
        EXIT_COUNTER( demux_discontinuity );
        EXIT_COUNTER( played_abuffers );
        EXIT_COUNTER( lost_abuffers );
        EXIT_COUNTER( displayed_pictures );
        EXIT_COUNTER( lost_pictures );
        EXIT_COUNTER( decoded_audio );
        EXIT_COUNTER( decoded_video );
        EXIT_COUNTER( decoded_sub );
        if( p_input->p->p_sout )
        {
            EXIT_COUNTER( sout_sent_packets );
            EXIT_COUNTER( sout_sent_bytes );
            EXIT_COUNTER( sout_send_bitrate );
        }
#undef EXIT_COUNTER
    }
    /* Mark them deleted */
    p_input->p->input.p_demux = NULL;
    p_input->p->p_es_out = NULL;
    p_input->p->p_sout = NULL;
    return VLC_EGENERIC;
}
