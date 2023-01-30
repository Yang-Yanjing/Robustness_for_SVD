}
addons_manager_t *addons_manager_New( vlc_object_t *p_this )
{
    addons_manager_t *p_manager = malloc( sizeof(addons_manager_t) );
    if ( !p_manager ) return NULL;
    p_manager->p_priv = malloc( sizeof(addons_manager_private_t) );
    if ( !p_manager->p_priv )
    {
        free( p_manager );
        return NULL;
    }
    p_manager->p_event_manager = malloc( sizeof(vlc_event_manager_t) );
    if ( !p_manager->p_event_manager )
    {
        free( p_manager->p_priv );
        free( p_manager );
        return NULL;
    }
    p_manager->p_priv->p_parent = p_this;
#define INIT_QUEUE( name ) \
    p_manager->p_priv->name.b_live = false;\
    vlc_mutex_init( &p_manager->p_priv->name.lock );\
    vlc_cond_init( &p_manager->p_priv->name.waitcond );\
    ARRAY_INIT( p_manager->p_priv->name.entries );
    INIT_QUEUE( finder )
    INIT_QUEUE( installer )
    ARRAY_INIT( p_manager->p_priv->finder.uris );
    vlc_event_manager_t *em = p_manager->p_event_manager;
    vlc_event_manager_init( em, p_manager );
    vlc_event_manager_register_event_type(em, vlc_AddonFound);
    vlc_event_manager_register_event_type(em, vlc_AddonsDiscoveryEnded);
    vlc_event_manager_register_event_type(em, vlc_AddonChanged);
    return p_manager;
}
