}
static int addons_manager_WriteCatalog( addons_manager_t *p_manager )
{
    int i_return = VLC_EGENERIC;
    addons_storage_t *p_storage =
        vlc_custom_create( p_manager->p_priv->p_parent, sizeof( *p_storage ), "entries storage" );
    p_storage->i_flags |= OBJECT_FLAGS_NOINTERACT;
    module_t *p_module = module_need( p_storage, "addons storage",
                                      "addons.store.install", true );
    if( p_module )
    {
        vlc_mutex_lock( &p_manager->p_priv->finder.lock );
        i_return = p_storage->pf_catalog( p_storage, p_manager->p_priv->finder.entries.p_elems,
                                          p_manager->p_priv->finder.entries.i_size );
        vlc_mutex_unlock( &p_manager->p_priv->finder.lock );
        module_unneed( p_storage, p_module );
    }
    vlc_object_release( p_storage );
    return i_return;
}
