}
static bool module_match(const module_t *m, const char *pattern, bool strict)
{
    if (pattern == NULL)
        return true;
    const char *objname = module_get_object(m);
    if (strict ? (strcmp(objname, pattern) == 0)
               : (strstr(objname, pattern) != NULL))
        return true;
    for (unsigned i = 0; i < m->i_shortcuts; i++)
    {
        const char *shortcut = m->pp_shortcuts[i];
        if (strict ? (strcmp(shortcut, pattern) == 0)
                   : (strstr(shortcut, pattern) != NULL))
            return true;
    }
    return false;
}
