}
module_t *vlc_module_create (module_t *parent)
{
    module_t *module = malloc (sizeof (*module));
    if (module == NULL)
        return NULL;
    /* TODO: replace module/submodules with plugin/modules */
    if (parent == NULL)
    {
        module->next = NULL;
        module->parent = NULL;
    }
    else
    {
        module->next = parent->submodule;
        parent->submodule = module;
        parent->submodule_count++;
        module->parent = parent;
    }
    module->submodule = NULL;
    module->submodule_count = 0;
    module->psz_shortname = NULL;
    module->psz_longname = NULL;
    module->psz_help = NULL;
    module->pp_shortcuts = NULL;
    module->i_shortcuts = 0;
    module->psz_capability = NULL;
    module->i_score = (parent != NULL) ? parent->i_score : 1;
    module->b_loaded = false;
    module->b_unloadable = parent == NULL;
    module->pf_activate = NULL;
    module->pf_deactivate = NULL;
    module->p_config = NULL;
    module->confsize = 0;
    module->i_config_items = 0;
    module->i_bool_items = 0;
    /*module->handle = garbage */
    module->psz_filename = NULL;
    module->domain = NULL;
    return module;
}
