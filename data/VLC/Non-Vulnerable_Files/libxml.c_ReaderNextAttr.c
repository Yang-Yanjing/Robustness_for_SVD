#endif
static const char *ReaderNextAttr( xml_reader_t *p_reader, const char **pval )
{
    xmlTextReaderPtr xml = p_reader->p_sys->xml;
    const xmlChar *name, *value;
    if( xmlTextReaderMoveToNextAttribute( xml ) != 1
     || (name = xmlTextReaderConstName( xml )) == NULL
     || (value = xmlTextReaderConstValue( xml )) == NULL )
        return NULL;
    *pval = (const char *)value;
    return (const char *)name;
}
