 */
module_t **module_list_get (size_t *n)
{
    module_t **tab = NULL;
    size_t i = 0;
    assert (n != NULL);
    for (module_t *mod = modules.head; mod; mod = mod->next)
    {
         module_t **nt;
         nt  = realloc (tab, (i + 1 + mod->submodule_count) * sizeof (*tab));
         if (unlikely(nt == NULL))
         {
             free (tab);
             *n = 0;
             return NULL;
         }
         tab = nt;
         tab[i++] = mod;
         for (module_t *subm = mod->submodule; subm; subm = subm->next)
             tab[i++] = subm;
    }
    *n = i;
    return tab;
}
