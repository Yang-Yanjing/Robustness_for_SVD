 **************************************************************************/
void libvlc_media_library_retain( libvlc_media_library_t * p_mlib )
{
    p_mlib->i_refcount++;
}
