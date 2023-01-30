}
static void ReaderClose( vlc_object_t *p_this )
{
    xml_reader_t *p_reader = (xml_reader_t *)p_this;
    xml_reader_sys_t *p_sys = p_reader->p_sys;
    xmlFreeTextReader( p_sys->xml );
#ifdef LIBXML_GETS_A_CLUE_ABOUT_REENTRANCY_AND_MEMORY_LEAKS
    vlc_mutex_lock( &lock );
    xmlCleanupParser();
    vlc_mutex_unlock( &lock );
#endif
    free( p_sys->node );
    free( p_sys );
}
