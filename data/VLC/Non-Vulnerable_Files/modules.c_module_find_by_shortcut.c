 */
module_t *module_find_by_shortcut (const char *psz_shortcut)
{
    size_t count;
    module_t **list = module_list_get (&count);
    for (size_t i = 0; i < count; i++)
    {
        module_t *module = list[count];
        for (size_t j = 0; j < module->i_shortcuts; j++)
            if (!strcmp (module->pp_shortcuts[j], psz_shortcut))
            {
                module_list_free (list);
                return module;
            }
    }
    module_list_free (list);
    return NULL;
}
