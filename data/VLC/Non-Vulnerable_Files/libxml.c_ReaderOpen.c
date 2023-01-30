};
static int ReaderOpen( vlc_object_t *p_this )
{
    if( !xmlHasFeature( XML_WITH_THREAD ) )
        return VLC_EGENERIC;
    xml_reader_t *p_reader = (xml_reader_t *)p_this;
    xml_reader_sys_t *p_sys = malloc( sizeof( *p_sys ) );
    xmlTextReaderPtr p_libxml_reader;
    if( unlikely(!p_sys) )
        return VLC_ENOMEM;
    vlc_mutex_lock( &lock );
    xmlInitParser();
    vlc_mutex_unlock( &lock );
    p_libxml_reader = xmlReaderForIO( StreamRead, NULL, p_reader->p_stream,
                                      NULL, NULL, 0 );
    if( !p_libxml_reader )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    /* Set the error handler */
    xmlTextReaderSetErrorHandler( p_libxml_reader,
                                  ReaderErrorHandler, p_reader );
    p_sys->xml = p_libxml_reader;
    p_sys->node = NULL;
    p_reader->p_sys = p_sys;
    p_reader->pf_next_node = ReaderNextNode;
    p_reader->pf_next_attr = ReaderNextAttr;
    p_reader->pf_is_empty = ReaderIsEmptyElement;
    p_reader->pf_use_dtd = ReaderUseDTD;
    return VLC_SUCCESS;
}
