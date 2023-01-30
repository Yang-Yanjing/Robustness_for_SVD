 **************************************************************************/
void libvlc_media_library_release( libvlc_media_library_t * p_mlib )
{
    p_mlib->i_refcount--;
    if( p_mlib->i_refcount > 0 )
        return;
    libvlc_event_manager_release( p_mlib->p_event_manager );
    free( p_mlib );
}
