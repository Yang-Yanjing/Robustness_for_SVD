 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
#ifdef LIBXML_GETS_A_CLUE_ABOUT_REENTRANCY_AND_MEMORY_LEAKS
    vlc_mutex_lock( &lock );
    xmlCleanupParser();
    vlc_mutex_unlock( &lock );
#endif
    VLC_UNUSED(p_this);
    return;
}
