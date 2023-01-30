 */
static module_t *module_InitStatic (vlc_plugin_cb entry)
{
    /* Initializes the module */
    module_t *module = vlc_plugin_describe (entry);
    if (unlikely(module == NULL))
        return NULL;
    module->b_loaded = true;
    module->b_unloadable = false;
    return module;
}
