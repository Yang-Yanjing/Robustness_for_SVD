 */
int Demux( demux_t *p_demux )
{
    xml_reader_t *p_xml_reader;
    const char *node;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    p_demux->p_sys->i_ntracks = 0;
    /* create new xml parser from stream */
    p_xml_reader = xml_ReaderCreate( p_demux, p_demux->s );
    if( !p_xml_reader )
        goto end;
    /* locating the root node */
    int type;
    do
    {
        type = xml_ReaderNextNode( p_xml_reader, &node );
        if( type <= 0 )
        {
            msg_Err( p_demux, "can't read xml stream" );
            goto end;
        }
    }
    while( type != XML_READER_STARTELEM );
    /* checking root node name */
    if( strcmp( node, "plist" ) )
    {
        msg_Err( p_demux, "invalid root node <%s>", node );
        goto end;
    }
    input_item_node_t *p_subitems = input_item_node_Create( p_current_input );
    xml_elem_hnd_t pl_elements[] =
        { {"dict",    COMPLEX_CONTENT, {.cmplx = parse_plist_dict} } };
    parse_plist_node( p_demux, p_subitems, NULL, p_xml_reader, "plist",
                      pl_elements );
    input_item_node_PostAndDelete( p_subitems );
    vlc_gc_decref(p_current_input);
end:
    if( p_xml_reader )
        xml_ReaderDelete( p_xml_reader );
    /* Needed for correct operation of go back */
    return 0;
}
