 *****************************************************************************/
static void ReaderErrorHandler( void *p_arg, const char *p_msg,
                                xmlParserSeverities severity,
                                xmlTextReaderLocatorPtr locator)
{
    VLC_UNUSED(severity);
    xml_reader_t *p_reader = (xml_reader_t *)p_arg;
    int line = xmlTextReaderLocatorLineNumber( locator );
    msg_Err( p_reader, "XML parser error (line %d) : %s", line, p_msg );
}
