 */
void module_InitBank (void)
{
    vlc_mutex_lock (&modules.lock);
    if (modules.usage == 0)
    {
        /* Fills the module bank structure with the core module infos.
         * This is very useful as it will allow us to consider the core
         * library just as another module, and for instance the configuration
         * options of core will be available in the module bank structure just
         * as for every other module. */
        module_t *module = module_InitStatic (vlc_entry__core);
        if (likely(module != NULL))
            module_StoreBank (module);
        config_SortConfig ();
    }
    modules.usage++;
    /* We do retain the module bank lock until the plugins are loaded as well.
     * This is ugly, this staged loading approach is needed: LibVLC gets
     * some configuration parameters relevant to loading the plugins from
     * the core (builtin) module. The module bank becomes shared read-only data
     * once it is ready, so we need to fully serialize initialization.
     * DO NOT UNCOMMENT the following line unless you managed to squeeze
     * module_LoadPlugins() before you unlock the mutex. */
    /*vlc_mutex_unlock (&modules.lock);*/
}
