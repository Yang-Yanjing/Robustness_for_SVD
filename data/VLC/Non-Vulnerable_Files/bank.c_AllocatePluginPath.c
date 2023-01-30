 */
static void AllocatePluginPath (vlc_object_t *p_this, const char *path,
                                cache_mode_t mode)
{
    module_bank_t bank;
    module_cache_t *cache = NULL;
    size_t count = 0;
    switch( mode )
    {
        case CACHE_USE:
            count = CacheLoad( p_this, path, &cache );
            break;
        case CACHE_RESET:
            CacheDelete( p_this, path );
            break;
        case CACHE_IGNORE:
            msg_Dbg( p_this, "ignoring plugins cache file" );
    }
    msg_Dbg( p_this, "recursively browsing `%s'", path );
    bank.obj = p_this;
    bank.base = path;
    bank.mode = mode;
    bank.cache = NULL;
    bank.i_cache = 0;
    bank.loaded_cache = cache;
    bank.i_loaded_cache = count;
    /* Don't go deeper than 5 subdirectories */
    AllocatePluginDir (&bank, 5, path, NULL);
    switch( mode )
    {
        case CACHE_USE:
            /* Discard unmatched cache entries */
            for( size_t i = 0; i < count; i++ )
            {
                if (cache[i].p_module != NULL)
                   vlc_module_destroy (cache[i].p_module);
                free (cache[i].path);
            }
            free( cache );
#ifdef __APPLE__
            break;
#endif
        case CACHE_RESET:
            CacheSave (p_this, path, bank.cache, bank.i_cache);
        case CACHE_IGNORE:
            break;
    }
}
