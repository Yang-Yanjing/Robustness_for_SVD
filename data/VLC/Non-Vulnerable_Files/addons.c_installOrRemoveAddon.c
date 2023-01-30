}
static int installOrRemoveAddon( addons_manager_t *p_manager, addon_entry_t *p_entry, bool b_install )
{
    int i_return = VLC_EGENERIC;
    addons_storage_t *p_storage =
        vlc_custom_create( p_manager->p_priv->p_parent, sizeof( *p_storage ), "entries storage" );
    p_storage->i_flags |= OBJECT_FLAGS_NOINTERACT;
    module_t *p_module = module_need( p_storage, "addons storage",
                                      "addons.store.install", true );
    if( p_module )
    {
        if ( b_install )
            i_return = p_storage->pf_install( p_storage, p_entry );
        else
            i_return = p_storage->pf_remove( p_storage, p_entry );
        module_unneed( p_storage, p_module );
        msg_Dbg( p_manager->p_priv->p_parent, "InstallAddon returns %d", i_return );
        if ( i_return == VLC_SUCCESS )
        {
            /* Reset flags */
            vlc_mutex_lock( &p_entry->lock );
            p_entry->e_flags = ADDON_MANAGEABLE;
            vlc_mutex_unlock( &p_entry->lock );
        }
    }
    vlc_object_release( p_storage );
    return i_return;
}
