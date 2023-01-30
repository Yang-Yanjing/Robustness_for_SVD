}
static int Install( addons_storage_t *p_storage, addon_entry_t *p_entry )
{
    vlc_object_t *p_this = VLC_OBJECT( p_storage );
    int i_ret = VLC_EGENERIC;
    if ( ! p_entry->psz_source_module )
        return i_ret;
    /* Query origin module for download path */
    addons_finder_t *p_finder = vlc_object_create( p_this, sizeof( addons_finder_t ) );
    if( !p_finder )
        return VLC_ENOMEM;
    module_t *p_module = module_need( p_finder, "addons finder",
                                      p_entry->psz_source_module, true );
    if( p_module )
    {
        if ( p_finder->pf_retrieve( p_finder, p_entry ) == VLC_SUCCESS )
        {
            /* Do things while retrieved data is here */
            vlc_mutex_lock( &p_entry->lock );
            i_ret = InstallAllFiles( p_storage, p_entry );
            vlc_mutex_unlock( &p_entry->lock );
            /* !Do things while retrieved data is here */
        }
        module_unneed( p_finder, p_module );
    }
    vlc_object_release( p_finder );
    return i_ret;
}
