static module_t *module_InitStatic (vlc_plugin_cb);
static void module_StoreBank (module_t *module)
{
    /*vlc_assert_locked (&modules.lock);*/
    module->next = modules.head;
    modules.head = module;
}
