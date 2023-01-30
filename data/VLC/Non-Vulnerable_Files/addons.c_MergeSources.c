}
static void MergeSources( addons_manager_t *p_manager,
                          addon_entry_t **pp_addons, int i_count )
{
    addon_entry_t *p_entry, *p_manager_entry;
    addon_uuid_t zerouuid;
    memset( zerouuid, 0, sizeof( addon_uuid_t ) );
    for ( int i=0; i < i_count; i++ )
    {
        p_entry = pp_addons[i];
        vlc_mutex_lock( &p_entry->lock );
        if ( memcmp( p_entry->uuid, zerouuid, sizeof( addon_uuid_t ) ) )
            p_manager_entry = getHeldEntryByUUID( p_manager, p_entry->uuid );
        else
            p_manager_entry = NULL;
        if ( !p_manager_entry )
        {
            ARRAY_APPEND( p_manager->p_priv->finder.entries, p_entry );
            vlc_event_t event;
            event.type = vlc_AddonFound;
            event.u.addon_generic_event.p_entry = p_entry;
            vlc_event_send( p_manager->p_event_manager, &event );
        }
        else
        {
            vlc_mutex_lock( &p_manager_entry->lock );
            if ( ( p_manager_entry->psz_version && p_entry->psz_version )
                 && /* FIXME: better version comparison */
                 strcmp( p_manager_entry->psz_version, p_entry->psz_version )
                 )
            {
                p_manager_entry->e_flags |= ADDON_UPDATABLE;
            }
            vlc_mutex_unlock( &p_manager_entry->lock );
            addon_entry_Release( p_manager_entry );
        }
        vlc_mutex_unlock( &p_entry->lock );
    }
}
