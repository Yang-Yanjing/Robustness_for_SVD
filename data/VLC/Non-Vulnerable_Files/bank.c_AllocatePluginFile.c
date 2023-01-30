 */
static int AllocatePluginFile (module_bank_t *bank, const char *abspath,
                               const char *relpath, const struct stat *st)
{
    module_t *module = NULL;
    /* Check our plugins cache first then load plugin if needed */
    if (bank->mode == CACHE_USE)
    {
        module = CacheFind (bank->loaded_cache, bank->i_loaded_cache,
                            relpath, st);
        if (module != NULL)
        {
            module->psz_filename = strdup (abspath);
            if (unlikely(module->psz_filename == NULL))
            {
                vlc_module_destroy (module);
                module = NULL;
            }
        }
    }
    if (module == NULL)
        module = module_InitDynamic (bank->obj, abspath, true);
    if (module == NULL)
        return -1;
    /* We have not already scanned and inserted this module */
    assert (module->next == NULL);
    /* Unload plugin until we really need it */
    if (module->b_loaded && module->b_unloadable)
    {
        module_Unload (module->handle);
        module->b_loaded = false;
    }
    /* For now we force loading if the module's config contains callbacks.
     * Could be optimized by adding an API call.*/
    for (size_t n = module->confsize, i = 0; i < n; i++)
         if (module->p_config[i].list_count == 0
          && (module->p_config[i].list.psz_cb != NULL || module->p_config[i].list.i_cb != NULL))
         {
             /* !unloadable not allowed for plugins with callbacks */
             vlc_module_destroy (module);
             module = module_InitDynamic (bank->obj, abspath, false);
             if (unlikely(module == NULL))
                 return -1;
             break;
         }
    module_StoreBank (module);
    if (bank->mode != CACHE_IGNORE) /* Add entry to cache */
        CacheAdd (&bank->cache, &bank->i_cache, relpath, st, module);
    /* TODO: deal with errors */
    return  0;
}
