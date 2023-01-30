 */
void vlc_module_destroy (module_t *module)
{
    assert (!module->b_loaded || !module->b_unloadable);
    for (module_t *m = module->submodule, *next; m != NULL; m = next)
    {
        next = m->next;
        vlc_module_destroy (m);
    }
    config_Free (module->p_config, module->confsize);
    free (module->domain);
    free (module->psz_filename);
    for (unsigned i = 0; i < module->i_shortcuts; i++)
        free (module->pp_shortcuts[i]);
    free (module->pp_shortcuts);
    free (module->psz_capability);
    free (module->psz_help);
    free (module->psz_longname);
    free (module->psz_shortname);
    free (module);
}
