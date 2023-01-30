 */
module_t *module_find (const char *name)
{
    size_t count;
    module_t **list = module_list_get (&count);
    assert (name != NULL);
    for (size_t i = 0; i < count; i++)
    {
        module_t *module = list[i];
        if (unlikely(module->i_shortcuts == 0))
            continue;
        if (!strcmp (module->pp_shortcuts[0], name))
        {
            module_list_free (list);
            return module;
        }
    }
    module_list_free (list);
    return NULL;
}
