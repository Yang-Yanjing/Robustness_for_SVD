 */
ssize_t module_list_cap (module_t ***restrict list, const char *cap)
{
    /* TODO: This is quite inefficient. List should be sorted by capability. */
    ssize_t n = 0;
    assert (list != NULL);
    for (module_t *mod = modules.head; mod != NULL; mod = mod->next)
    {
         if (module_provides (mod, cap))
             n++;
         for (module_t *subm = mod->submodule; subm != NULL; subm = subm->next)
             if (module_provides (subm, cap))
                 n++;
    }
    module_t **tab = malloc (sizeof (*tab) * n);
    *list = tab;
    if (unlikely(tab == NULL))
        return -1;
    for (module_t *mod = modules.head; mod != NULL; mod = mod->next)
    {
         if (module_provides (mod, cap))
             *(tab++)= mod;
         for (module_t *subm = mod->submodule; subm != NULL; subm = subm->next)
             if (module_provides (subm, cap))
                 *(tab++) = subm;
    }
    assert (tab == *list + n);
    qsort (*list, n, sizeof (*tab), modulecmp);
    return n;
}
