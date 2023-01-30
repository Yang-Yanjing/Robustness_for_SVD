libvlc_media_library_t *
libvlc_media_library_new( libvlc_instance_t * p_inst )
{
    libvlc_media_library_t * p_mlib;
    p_mlib = malloc(sizeof(libvlc_media_library_t));
    if( !p_mlib )
    {
        libvlc_printerr( "Not enough memory" );
        return NULL;
    }
    p_mlib->p_libvlc_instance = p_inst;
    p_mlib->i_refcount = 1;
    p_mlib->p_mlist = NULL;
    p_mlib->p_event_manager = libvlc_event_manager_new( p_mlib, p_inst );
    if( unlikely(p_mlib->p_event_manager == NULL) )
    {
        free(p_mlib);
        return NULL;
    }
    return p_mlib;
}
