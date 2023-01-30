}
static int ParseSkins2Info( addons_finder_t *p_finder, stream_t *p_stream,
                            char **ppsz_title, char **ppsz_source )
{
    const char *p_node;
    int i_current_node_type;
    bool b_done = false;
    xml_reader_t *p_xml_reader = xml_ReaderCreate( p_finder, p_stream );
    if( !p_xml_reader ) return VLC_EGENERIC;
    if( xml_ReaderNextNode( p_xml_reader, &p_node ) != XML_READER_STARTELEM )
    {
        msg_Err( p_finder, "invalid xml file" );
        goto error;
    }
    if ( strcmp( p_node, "Theme") )
    {
        msg_Err( p_finder, "unsupported XML data format" );
        goto error;
    }
    while( !b_done && (i_current_node_type = xml_ReaderNextNode( p_xml_reader, &p_node )) > 0 )
    {
        switch( i_current_node_type )
        {
        case XML_READER_STARTELEM:
        {
            if ( !strcmp( p_node, "ThemeInfo" ) )
            {
                const char *attr, *value;
                while( (attr = xml_ReaderNextAttr( p_xml_reader, &value )) )
                {
                    if ( !strcmp( attr, "name" ) )
                        *ppsz_title = strdup( value );
                    else if ( !strcmp( attr, "webpage" ) )
                        *ppsz_source = strdup( value );
                }
                b_done = true;
            }
            break;
        }
        default:
            break;
        }
    }
    xml_ReaderDelete( p_xml_reader );
    return ( b_done ) ? VLC_SUCCESS : VLC_EGENERIC;
error:
    xml_ReaderDelete( p_xml_reader );
    return VLC_EGENERIC;
}
