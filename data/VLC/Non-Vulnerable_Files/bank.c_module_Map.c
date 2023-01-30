 */
int module_Map (vlc_object_t *obj, module_t *module)
{
    if (module->parent != NULL)
        module = module->parent;
#warning FIXME: race condition!
    if (module->b_loaded)
        return 0;
    assert (module->psz_filename != NULL);
#ifdef HAVE_DYNAMIC_PLUGINS
    module_t *uncache = module_InitDynamic (obj, module->psz_filename, false);
    if (uncache != NULL)
    {
        CacheMerge (obj, module, uncache);
        vlc_module_destroy (uncache);
        return 0;
    }
#endif
    msg_Err (obj, "corrupt module: %s", module->psz_filename);
    return -1;
}
