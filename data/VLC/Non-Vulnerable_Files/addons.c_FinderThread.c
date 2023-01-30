}
static void *FinderThread( void *p_data )
{
    addons_manager_t *p_manager = p_data;
    int i_cancel;
    char *psz_uri;
    for( ;; )
    {
        vlc_mutex_lock( &p_manager->p_priv->finder.lock );
        mutex_cleanup_push( &p_manager->p_priv->finder.lock );
        while( p_manager->p_priv->finder.uris.i_size == 0 )
        {
            vlc_cond_wait( &p_manager->p_priv->finder.waitcond,
                           &p_manager->p_priv->finder.lock );
        }
        psz_uri = p_manager->p_priv->finder.uris.p_elems[0];
        ARRAY_REMOVE( p_manager->p_priv->finder.uris, 0 );
        vlc_cleanup_run();
        addons_finder_t *p_finder =
                vlc_custom_create( p_manager->p_priv->p_parent, sizeof( *p_finder ), "entries finder" );
        i_cancel = vlc_savecancel();
        if( p_finder != NULL )
        {
            p_finder->i_flags |= OBJECT_FLAGS_NOINTERACT;
            module_t *p_module;
            ARRAY_INIT( p_finder->entries );
            vlc_mutex_lock( &p_manager->p_priv->finder.lock );
            p_finder->psz_uri = psz_uri;
            vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
            p_module = module_need( p_finder, "addons finder", NULL, false );
            if( p_module )
            {
                p_finder->pf_find( p_finder );
                module_unneed( p_finder, p_module );
                MergeSources( p_manager, p_finder->entries.p_elems, p_finder->entries.i_size );
            }
            ARRAY_RESET( p_finder->entries );
            free( psz_uri );
            vlc_object_release( p_finder );
        }
        vlc_event_t event;
        event.type = vlc_AddonsDiscoveryEnded;
        event.u.addon_generic_event.p_entry = NULL;
        vlc_event_send( p_manager->p_event_manager, &event );
        vlc_restorecancel( i_cancel );
        vlc_testcancel();
    }
    return NULL;
}
