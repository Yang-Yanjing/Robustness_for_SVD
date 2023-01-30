 **/
static int DemuxGenre( demux_t *p_demux, xml_reader_t *p_xml_reader,
                       input_item_node_t *p_input_node )
{
    const char *node;
    char *psz_name = NULL; /* genre name */
    int type;
    while( (type = xml_ReaderNextNode( p_xml_reader, &node )) > 0 )
    {
        switch( type )
        {
            case XML_READER_STARTELEM:
            {
                if( !strcmp( node, "genre" ) )
                {
                    // Read the attributes
                    const char *name, *value;
                    while( (name = xml_ReaderNextAttr( p_xml_reader, &value )) )
                    {
                        if( !strcmp( name, "name" ) )
                        {
                            free(psz_name);
                            psz_name = strdup( value );
                        }
                        else
                            msg_Warn( p_demux,
                                      "unexpected attribute %s in <%s>",
                                      name, node );
                    }
                }
                break;
            }
            case XML_READER_ENDELEM:
                if( !strcmp( node, "genre" ) && psz_name != NULL )
                {
                    char* psz_mrl;
                    if( asprintf( &psz_mrl, SHOUTCAST_BASE_URL "?genre=%s",
                                  psz_name ) != -1 )
                    {
                        input_item_t *p_input;
                        resolve_xml_special_chars( psz_mrl );
                        p_input = input_item_New( psz_mrl, psz_name );
                        input_item_CopyOptions( p_input_node->p_item, p_input );
                        free( psz_mrl );
                        input_item_node_AppendItem( p_input_node, p_input );
                        vlc_gc_decref( p_input );
                    }
                    FREENULL( psz_name );
                }
                break;
        }
    }
    free( psz_name );
    return 0;
}
