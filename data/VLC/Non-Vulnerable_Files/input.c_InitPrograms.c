}
static void InitPrograms( input_thread_t * p_input )
{
    int i_es_out_mode;
    vlc_list_t list;
    /* Compute correct pts_delay */
    UpdatePtsDelay( p_input );
    /* Set up es_out */
    i_es_out_mode = ES_OUT_MODE_AUTO;
    if( p_input->p->p_sout )
    {
        char *prgms;
        if( var_GetBool( p_input, "sout-all" ) )
        {
            i_es_out_mode = ES_OUT_MODE_ALL;
        }
        else
        if( (prgms = var_GetNonEmptyString( p_input, "programs" )) != NULL )
        {
            char *buf;
            TAB_INIT( list.i_count, list.p_values );
            for( const char *prgm = strtok_r( prgms, ",", &buf );
                 prgm != NULL;
                 prgm = strtok_r( NULL, ",", &buf ) )
            {
                vlc_value_t val = { .i_int = atoi( prgm ) };
                INSERT_ELEM( list.p_values, list.i_count, list.i_count, val );
            }
            if( list.i_count > 0 )
                i_es_out_mode = ES_OUT_MODE_PARTIAL;
                /* Note : we should remove the "program" callback. */
            free( prgms );
        }
    }
    es_out_SetMode( p_input->p->p_es_out, i_es_out_mode );
    /* Inform the demuxer about waited group (needed only for DVB) */
    if( i_es_out_mode == ES_OUT_MODE_ALL )
    {
        demux_Control( p_input->p->input.p_demux, DEMUX_SET_GROUP, -1, NULL );
    }
    else if( i_es_out_mode == ES_OUT_MODE_PARTIAL )
    {
        demux_Control( p_input->p->input.p_demux, DEMUX_SET_GROUP, -1,
                       &list );
        TAB_CLEAN( list.i_count, list.p_values );
    }
    else
    {
        demux_Control( p_input->p->input.p_demux, DEMUX_SET_GROUP,
                       es_out_GetGroupForced( p_input->p->p_es_out ), NULL );
    }
}
