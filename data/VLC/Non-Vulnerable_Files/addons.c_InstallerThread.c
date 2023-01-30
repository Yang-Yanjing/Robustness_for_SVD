}
static void *InstallerThread( void *p_data )
{
    addons_manager_t *p_manager = p_data;
    int i_ret, i_cancel;
    vlc_event_t event;
    event.type = vlc_AddonChanged;
    for( ;; )
    {
        vlc_mutex_lock( &p_manager->p_priv->installer.lock );
        mutex_cleanup_push( &p_manager->p_priv->installer.lock );
        while ( !p_manager->p_priv->installer.entries.i_size )
        {
            /* No queued addons */
            vlc_cond_wait( &p_manager->p_priv->installer.waitcond,
                           &p_manager->p_priv->installer.lock );
        }
        vlc_cleanup_pop();
        addon_entry_t *p_entry = p_manager->p_priv->installer.entries.p_elems[0];
        ARRAY_REMOVE( p_manager->p_priv->installer.entries, 0 );
        addon_entry_Hold( p_entry );
        vlc_mutex_unlock( &p_manager->p_priv->installer.lock );
        vlc_mutex_lock( &p_entry->lock );
        /* DO WORK */
        if ( p_entry->e_state == ADDON_INSTALLED )
        {
            p_entry->e_state = ADDON_UNINSTALLING;
            vlc_mutex_unlock( &p_entry->lock );
            /* notify */
            i_cancel = vlc_savecancel();
            event.u.addon_generic_event.p_entry = p_entry;
            vlc_event_send( p_manager->p_event_manager, &event );
            i_ret = installOrRemoveAddon( p_manager, p_entry, false );
            vlc_restorecancel( i_cancel );
            vlc_mutex_lock( &p_entry->lock );
            p_entry->e_state = ( i_ret == VLC_SUCCESS ) ? ADDON_NOTINSTALLED
                                                        : ADDON_INSTALLED;
            vlc_mutex_unlock( &p_entry->lock );
        }
        else if ( p_entry->e_state == ADDON_NOTINSTALLED )
        {
            p_entry->e_state = ADDON_INSTALLING;
            vlc_mutex_unlock( &p_entry->lock );
            /* notify */
            i_cancel = vlc_savecancel();
            event.u.addon_generic_event.p_entry = p_entry;
            vlc_event_send( p_manager->p_event_manager, &event );
            i_ret = installOrRemoveAddon( p_manager, p_entry, true );
            vlc_restorecancel( i_cancel );
            vlc_mutex_lock( &p_entry->lock );
            p_entry->e_state = ( i_ret == VLC_SUCCESS ) ? ADDON_INSTALLED
                                                        : ADDON_NOTINSTALLED;
            vlc_mutex_unlock( &p_entry->lock );
        }
        else
            vlc_mutex_unlock( &p_entry->lock );
        /* !DO WORK */
        i_cancel = vlc_savecancel();
        event.u.addon_generic_event.p_entry = p_entry;
        vlc_event_send( p_manager->p_event_manager, &event );
        vlc_restorecancel( i_cancel );
        addon_entry_Release( p_entry );
        i_cancel = vlc_savecancel();
        addons_manager_WriteCatalog( p_manager );
        vlc_restorecancel( i_cancel );
    }
    return NULL;
}
