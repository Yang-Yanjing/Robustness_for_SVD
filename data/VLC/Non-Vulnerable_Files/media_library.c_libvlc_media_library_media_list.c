libvlc_media_list_t *
libvlc_media_library_media_list( libvlc_media_library_t * p_mlib )
{
    if( p_mlib->p_mlist )
        libvlc_media_list_retain( p_mlib->p_mlist );
    return p_mlib->p_mlist;
}
