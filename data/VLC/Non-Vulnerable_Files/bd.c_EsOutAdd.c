};
static es_out_id_t *EsOutAdd( es_out_t *p_out, const es_format_t *p_fmt )
{
    demux_t *p_demux = p_out->p_sys->p_demux;
    const bd_mpls_t *p_mpls = p_demux->p_sys->pp_mpls[p_demux->info.i_title];
    const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[p_demux->p_sys->i_play_item];
    es_format_t fmt;
    es_format_Copy( &fmt, p_fmt );
    fmt.i_priority = ES_PRIORITY_NOT_SELECTABLE;
    for( int i = 0; i < p_item->i_stream; i++ )
    {
        const bd_mpls_stream_t *p_stream = &p_item->p_stream[i];
        if( p_stream->i_type != BD_MPLS_STREAM_TYPE_PLAY_ITEM ||
            p_stream->play_item.i_pid != fmt.i_id )
            continue;
        /* TODO improved priority for higher quality stream ?
         * if so, extending stream attributes parsing might be a good idea
         */
        fmt.i_priority = ES_PRIORITY_SELECTABLE_MIN;
#if 0
        /* Useless, and beside not sure it is the right thing to do */
        free( fmt.psz_description );
        switch( p_stream->i_class )
        {
        case BD_MPLS_STREAM_CLASS_SECONDARY_AUDIO:
            fmt.psz_description = strdup( "Secondary audio" );
            break;
        default:
            fmt.psz_description = NULL;
            break;
        }
#endif
        //msg_Err( p_demux, "Found ref for stream pid %d", fmt.i_id );
        if( *p_stream->psz_language && ( !fmt.psz_language || *fmt.psz_language == '\0' ) )
        {
            free( fmt.psz_language );
            fmt.psz_language = strdup( p_stream->psz_language );
        }
        switch( p_stream->i_charset )
        {
        /* TODO add all values */
        default:
            break;
        }
        break;
    }
    if( fmt.i_priority < ES_PRIORITY_SELECTABLE_MIN )
        msg_Dbg( p_demux, "Hiding one stream (pid=%d)", fmt.i_id );
    /* */
    es_out_id_t *p_es = es_out_Add( p_demux->out, &fmt );
    es_format_Clean( &fmt );
    return p_es;
}
