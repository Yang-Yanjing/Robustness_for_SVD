 */
static bool parse_plist_node( demux_t *p_demux, input_item_node_t *p_input_node,
                              track_elem_t *p_track, xml_reader_t *p_xml_reader,
                              const char *psz_element,
                              xml_elem_hnd_t *p_handlers )
{
    VLC_UNUSED(p_track); VLC_UNUSED(psz_element);
    const char *attr, *value;
    bool b_version_found = false;
    /* read all playlist attributes */
    while( (attr = xml_ReaderNextAttr( p_xml_reader, &value )) != NULL )
    {
        /* attribute: version */
        if( !strcmp( attr, "version" ) )
        {
            b_version_found = true;
            if( strcmp( value, "1.0" ) )
                msg_Warn( p_demux, "unsupported iTunes Media Library version" );
        }
        /* unknown attribute */
        else
            msg_Warn( p_demux, "invalid <plist> attribute:\"%s\"", attr );
    }
    /* attribute version is mandatory !!! */
    if( !b_version_found )
        msg_Warn( p_demux, "<plist> requires \"version\" attribute" );
    return parse_dict( p_demux, p_input_node, NULL, p_xml_reader,
                       "plist", p_handlers );
}
