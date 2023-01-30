 */
static bool parse_dict( demux_t *p_demux, input_item_node_t *p_input_node,
                        track_elem_t *p_track, xml_reader_t *p_xml_reader,
                        const char *psz_element, xml_elem_hnd_t *p_handlers )
{
    int i_node;
    const char *node;
    char *psz_value = NULL;
    char *psz_key = NULL;
    xml_elem_hnd_t *p_handler = NULL;
    bool b_ret = false;
    while( (i_node = xml_ReaderNextNode( p_xml_reader, &node )) > 0 )
    {
        switch( i_node )
        {
        /*  element start tag  */
        case XML_READER_STARTELEM:
            if( !*node )
            {
                msg_Err( p_demux, "invalid XML stream" );
                goto end;
            }
            /* choose handler */
            for( p_handler = p_handlers;
                     p_handler->name && strcmp( node, p_handler->name );
                     p_handler++ );
            if( !p_handler->name )
            {
                msg_Err( p_demux, "unexpected element <%s>", node );
                goto end;
            }
            /* complex content is parsed in a separate function */
            if( p_handler->type == COMPLEX_CONTENT )
            {
                if( p_handler->pf_handler.cmplx( p_demux, p_input_node, NULL,
                                                 p_xml_reader, p_handler->name,
                                                 NULL ) )
                {
                    p_handler = NULL;
                    FREE_ATT_KEY();
                }
                else
                    goto end;
            }
            break;
        /* simple element content */
        case XML_READER_TEXT:
            free( psz_value );
            psz_value = strdup( node );
            if( unlikely(!psz_value) )
                goto end;
            break;
        /* element end tag */
        case XML_READER_ENDELEM:
            /* leave if the current parent node <track> is terminated */
            if( !strcmp( node, psz_element ) )
            {
                b_ret = true;
                goto end;
            }
            /* there MUST have been a start tag for that element name */
            if( !p_handler || !p_handler->name
                || strcmp( p_handler->name, node ) )
            {
                msg_Err( p_demux, "there's no open element left for <%s>",
                         node );
                goto end;
            }
            /* special case: key */
            if( !strcmp( p_handler->name, "key" ) )
            {
                free( psz_key );
                psz_key = strdup( psz_value );
            }
            /* call the simple handler */
            else if( p_handler->pf_handler.smpl )
            {
                p_handler->pf_handler.smpl( p_track, psz_key, psz_value );
            }
            FREE_ATT();
            p_handler = NULL;
            break;
        }
    }
    msg_Err( p_demux, "unexpected end of XML data" );
end:
    free( psz_value );
    free( psz_key );
    return b_ret;
}
