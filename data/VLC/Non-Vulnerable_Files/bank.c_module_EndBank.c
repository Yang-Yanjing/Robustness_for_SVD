 */
void module_EndBank (bool b_plugins)
{
    module_t *head = NULL;
    /* If plugins were _not_ loaded, then the caller still has the bank lock
     * from module_InitBank(). */
    if( b_plugins )
        vlc_mutex_lock (&modules.lock);
    /*else
        vlc_assert_locked (&modules.lock); not for static mutexes :( */
    assert (modules.usage > 0);
    if (--modules.usage == 0)
    {
        config_UnsortConfig ();
        head = modules.head;
        modules.head = NULL;
    }
    vlc_mutex_unlock (&modules.lock);
    while (head != NULL)
    {
        module_t *module = head;
        head = module->next;
#ifdef HAVE_DYNAMIC_PLUGINS
        if (module->b_loaded && module->b_unloadable)
        {
            module_Unload (module->handle);
            module->b_loaded = false;
        }
#endif
        vlc_module_destroy (module);
    }
}
