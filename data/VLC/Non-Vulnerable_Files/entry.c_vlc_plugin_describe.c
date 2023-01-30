 */
module_t *vlc_plugin_describe (vlc_plugin_cb entry)
{
    module_t *module = NULL;
    if (entry (vlc_plugin_setter, &module) != 0)
    {
        if (module != NULL) /* partially initialized plug-in... */
            vlc_module_destroy (module);
        module = NULL;
    }
    return module;
}
