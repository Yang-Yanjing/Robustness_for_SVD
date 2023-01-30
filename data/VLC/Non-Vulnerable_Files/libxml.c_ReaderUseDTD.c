}
static int ReaderUseDTD ( xml_reader_t *p_reader )
{
    /* Activate DTD validation */
    xmlTextReaderSetParserProp( p_reader->p_sys->xml,
                                XML_PARSER_DEFAULTATTRS, true );
    xmlTextReaderSetParserProp( p_reader->p_sys->xml,
                                XML_PARSER_VALIDATE, true );
    return VLC_SUCCESS;
}
