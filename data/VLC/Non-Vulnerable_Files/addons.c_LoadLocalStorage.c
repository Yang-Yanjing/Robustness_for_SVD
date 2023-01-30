}
static void LoadLocalStorage( addons_manager_t *p_manager )
{
    addons_finder_t *p_finder =
        vlc_custom_create( p_manager->p_priv->p_parent, sizeof( *p_finder ), "entries finder" );
    p_finder->i_flags |= OBJECT_FLAGS_NOINTERACT;
    module_t *p_module = module_need( p_finder, "addons finder",
                                      "addons.store.list", true );
    if( p_module )
    {
        ARRAY_INIT( p_finder->entries );
        p_finder->psz_uri = NULL;
        p_finder->pf_find( p_finder );
        module_unneed( p_finder, p_module );
        MergeSources( p_manager, p_finder->entries.p_elems, p_finder->entries.i_size );
        ARRAY_RESET( p_finder->entries );
    }
    vlc_object_release( p_finder );
}
