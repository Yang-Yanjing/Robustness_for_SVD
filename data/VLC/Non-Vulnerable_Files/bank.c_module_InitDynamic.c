 */
static module_t *module_InitDynamic (vlc_object_t *obj, const char *path,
                                     bool fast)
{
    module_handle_t handle;
    if (module_Load (obj, path, &handle, fast))
        return NULL;
    /* Try to resolve the symbol */
    static const char entry_name[] = EXTERN_PREFIX "vlc_entry" MODULE_SUFFIX;
    vlc_plugin_cb entry =
        (vlc_plugin_cb) module_Lookup (handle, entry_name);
    if (entry == NULL)
    {
        msg_Warn (obj, "cannot find plug-in entry point in %s", path);
        goto error;
    }
    /* We can now try to call the symbol */
    module_t *module = vlc_plugin_describe (entry);
    if (unlikely(module == NULL))
    {
        /* With a well-written module we shouldn't have to print an
         * additional error message here, but just make sure. */
        msg_Err (obj, "cannot initialize plug-in %s", path);
        goto error;
    }
    module->psz_filename = strdup (path);
    if (unlikely(module->psz_filename == NULL))
    {
        vlc_module_destroy (module);
        goto error;
    }
    module->handle = handle;
    module->b_loaded = true;
    return module;
error:
    module_Unload( handle );
    return NULL;
}
