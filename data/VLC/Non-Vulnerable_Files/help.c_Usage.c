}
static void Usage (vlc_object_t *p_this, char const *psz_search)
{
    bool b_has_advanced = false;
    bool found = false;
    unsigned i_only_advanced = 0; /* Number of modules ignored because they
                               * only have advanced options */
    bool strict = false;
    if (psz_search != NULL && psz_search[0] == '=')
    {
        strict = true;
        psz_search++;
    }
    bool color = false;
#ifndef _WIN32
    if (isatty(STDOUT_FILENO))
        color = var_InheritBool(p_this, "color");
#endif
    const bool desc = var_InheritBool(p_this, "help-verbose");
    const bool advanced = var_InheritBool(p_this, "advanced");
    /* List all modules */
    size_t count;
    module_t **list = module_list_get (&count);
    /* Enumerate the config for each module */
    for (size_t i = 0; i < count; i++)
    {
        const module_t *m = list[i];
        const module_config_t *section = NULL;
        const char *objname = module_get_object(m);
        if (m->i_config_items == 0)
            continue; /* Ignore modules without config options */
        if (!module_match(m, psz_search, strict))
            continue;
        found = true;
        if (!module_show(m, advanced))
        {   /* Ignore modules with only advanced config options if requested */
            i_only_advanced++;
            continue;
        }
        /* Print name of module */
        printf(color ? "\n " GREEN "%s" GRAY " (%s)\n" : "\n %s (%s)\n",
               module_gettext(m, m->psz_longname), objname);
        if (m->psz_help != NULL)
            printf(color ? CYAN" %s\n"GRAY : " %s\n",
                   module_gettext(m, m->psz_help));
        /* Print module options */
        for (size_t i = 0; i < m->confsize; i++)
        {
            const module_config_t *item = m->p_config + i;
            if (item->b_removed)
                continue; /* Skip removed options */
            if (item->b_advanced && !advanced)
            {   /* Skip advanced options unless requested */
                b_has_advanced = true;
                continue;
            }
            print_item(m, item, &section, color, desc);
        }
    }
    if( b_has_advanced )
        printf(color ? "\n" WHITE "%s" GRAY " %s\n"
                     : "\n%s %s\n", _( "Note:" ), _( "add --advanced to your "
                                     "command line to see advanced options."));
    if( i_only_advanced > 0 )
    {
        printf(color ? "\n" WHITE "%s" GRAY " " : "\n%s ", _( "Note:" ) );
        printf(vlc_ngettext("%u module was not displayed because it only has "
               "advanced options.\n", "%u modules were not displayed because "
               "they only have advanced options.\n", i_only_advanced),
               i_only_advanced);
    }
    else if (!found)
        printf(color ? "\n" WHITE "%s" GRAY "\n" : "\n%s\n",
               _("No matching module found. Use --list or "
                 "--list-verbose to list available modules."));
    /* Release the module list */
    module_list_free (list);
}
