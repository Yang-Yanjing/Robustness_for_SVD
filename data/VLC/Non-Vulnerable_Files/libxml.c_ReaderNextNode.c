}
static int ReaderNextNode( xml_reader_t *p_reader, const char **pval )
{
    xml_reader_sys_t *p_sys = p_reader->p_sys;
    const xmlChar *node;
    int ret;
    free( p_sys->node );
    p_sys->node = NULL;
skip:
    switch( xmlTextReaderRead( p_sys->xml ) )
    {
        case 0: /* EOF */
            return XML_READER_NONE;
        case -1: /* error */
            return -1;
    }
    switch( xmlTextReaderNodeType( p_sys->xml ) )
    {
        case XML_READER_TYPE_ELEMENT:
            node = xmlTextReaderConstName( p_sys->xml );
            ret = XML_READER_STARTELEM;
            break;
        case XML_READER_TYPE_END_ELEMENT:
            node = xmlTextReaderConstName( p_sys->xml );
            ret = XML_READER_ENDELEM;
            break;
        case XML_READER_TYPE_CDATA:
        case XML_READER_TYPE_TEXT:
            node = xmlTextReaderConstValue( p_sys->xml );
            ret = XML_READER_TEXT;
            break;
        case -1:
            return -1;
        default:
            goto skip;
    }
    if( unlikely(node == NULL) )
        return -1;
    p_sys->node = strdup( (const char *)node );
    if( pval != NULL )
        *pval = p_sys->node;
    return likely(p_sys->node != NULL) ? ret : -1;
}
