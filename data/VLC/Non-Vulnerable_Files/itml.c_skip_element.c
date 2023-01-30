 */
static bool skip_element( demux_t *p_demux, input_item_node_t *p_input_node,
                          track_elem_t *p_track, xml_reader_t *p_xml_reader,
                          const char *psz_element, xml_elem_hnd_t *p_handlers )
{
    VLC_UNUSED(p_demux); VLC_UNUSED(p_input_node);
    VLC_UNUSED(p_track); VLC_UNUSED(p_handlers);
    const char *node;
    int type;
    while( (type = xml_ReaderNextNode( p_xml_reader, &node )) > 0 )
        if( type == XML_READER_ENDELEM && !strcmp( psz_element, node ) )
            return true;
    return false;
}
