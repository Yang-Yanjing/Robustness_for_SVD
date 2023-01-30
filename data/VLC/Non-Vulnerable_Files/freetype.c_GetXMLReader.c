}
static xml_reader_t *GetXMLReader( filter_t *p_filter, stream_t *p_sub )
{
    xml_reader_t *p_xml_reader = p_filter->p_sys->p_xml;
    if( !p_xml_reader )
        p_xml_reader = xml_ReaderCreate( p_filter, p_sub );
    else
        p_xml_reader = xml_ReaderReset( p_xml_reader, p_sub );
    p_filter->p_sys->p_xml = p_xml_reader;
    return p_xml_reader;
}
