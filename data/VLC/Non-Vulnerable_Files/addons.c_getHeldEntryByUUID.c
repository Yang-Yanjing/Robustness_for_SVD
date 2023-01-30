 *****************************************************************************/
static addon_entry_t * getHeldEntryByUUID( addons_manager_t *p_manager,
                                           const addon_uuid_t uuid )
{
    addon_entry_t *p_return = NULL;
    vlc_mutex_lock( &p_manager->p_priv->finder.lock );
    FOREACH_ARRAY( addon_entry_t *p_entry, p_manager->p_priv->finder.entries )
    if ( !memcmp( p_entry->uuid, uuid, sizeof( addon_uuid_t ) ) )
    {
        p_return = p_entry;
        addon_entry_Hold( p_return );
        break;
    }
    FOREACH_END()
    vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
    return p_return;
}
