}
void addons_manager_Delete( addons_manager_t *p_manager )
{
    bool b_live;
    vlc_mutex_lock( &p_manager->p_priv->finder.lock );
    b_live = p_manager->p_priv->finder.b_live;
    vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
    if ( b_live )
    {
        vlc_cancel( p_manager->p_priv->finder.thread );
        vlc_join( p_manager->p_priv->finder.thread, NULL );
    }
    vlc_mutex_lock( &p_manager->p_priv->installer.lock );
    b_live = p_manager->p_priv->installer.b_live;
    vlc_mutex_unlock( &p_manager->p_priv->installer.lock );
    if ( b_live )
    {
        vlc_cancel( p_manager->p_priv->installer.thread );
        vlc_join( p_manager->p_priv->installer.thread, NULL );
    }
    vlc_event_manager_fini( p_manager->p_event_manager );
#define FREE_QUEUE( name ) \
    vlc_mutex_lock( &p_manager->p_priv->name.lock );\
    FOREACH_ARRAY( addon_entry_t *p_entry, p_manager->p_priv->name.entries )\
        addon_entry_Release( p_entry );\
    FOREACH_END();\
    ARRAY_RESET( p_manager->p_priv->name.entries );\
    vlc_mutex_unlock( &p_manager->p_priv->name.lock );\
    vlc_mutex_destroy( &p_manager->p_priv->name.lock );\
    vlc_cond_destroy( &p_manager->p_priv->name.waitcond );
    FREE_QUEUE( finder )
    FREE_QUEUE( installer )
    FOREACH_ARRAY( char *psz_uri, p_manager->p_priv->finder.uris )
       free( psz_uri );
    FOREACH_END();
    ARRAY_RESET( p_manager->p_priv->finder.uris );
    free( p_manager->p_priv );
    free( p_manager->p_event_manager );
    free( p_manager );
}
