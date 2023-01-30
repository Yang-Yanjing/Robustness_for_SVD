}
static int Demux( demux_t *p_demux )
{
    xml_reader_t *p_xml_reader = NULL;
    const char *node;
    int i_ret = -1;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    input_item_node_t *p_input_node = NULL;
    p_xml_reader = xml_ReaderCreate( p_demux, p_demux->s );
    if( !p_xml_reader )
        goto error;
    /* check root node */
    if( xml_ReaderNextNode( p_xml_reader, &node ) != XML_READER_STARTELEM )
    {
        msg_Err( p_demux, "invalid file (no root node)" );
        goto error;
    }
    if( strcmp( node, "genrelist" ) && strcmp( node, "stationlist" ) )
    {
        msg_Err( p_demux, "invalid root node <%s>", node );
        goto error;
    }
    p_input_node = input_item_node_Create( p_current_input );
    if( !strcmp( node, "genrelist" ) )
    {
        /* we're reading a genre list */
        if( DemuxGenre( p_demux, p_xml_reader, p_input_node ) )
            goto error;
    }
    else
    {
        /* we're reading a station list */
        if( DemuxStation( p_demux, p_xml_reader, p_input_node,
                var_InheritBool( p_demux, "shoutcast-show-adult" ) ) )
            goto error;
    }
    input_item_node_PostAndDelete( p_input_node );
    p_input_node = NULL;
    i_ret = 0; /* Needed for correct operation of go back */
error:
    if( p_xml_reader )
        xml_ReaderDelete( p_xml_reader );
    if( p_input_node ) input_item_node_Delete( p_input_node );
    vlc_gc_decref(p_current_input);
    return i_ret;
}
