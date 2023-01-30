 */
size_t module_LoadPlugins (vlc_object_t *obj)
{
    /*vlc_assert_locked (&modules.lock); not for static mutexes :( */
    if (modules.usage == 1)
    {
        module_InitStaticModules ();
#ifdef HAVE_DYNAMIC_PLUGINS
        msg_Dbg (obj, "searching plug-in modules");
        AllocateAllPlugins (obj);
#endif
        config_UnsortConfig ();
        config_SortConfig ();
    }
    vlc_mutex_unlock (&modules.lock);
    size_t count;
    module_t **list = module_list_get (&count);
    module_list_free (list);
    msg_Dbg (obj, "plug-ins loaded: %zu modules", count);
    return count;
}
