}
static int ReaderIsEmptyElement( xml_reader_t *p_reader )
{
    return xmlTextReaderIsEmptyElement( p_reader->p_sys->xml );
}
