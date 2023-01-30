 *****************************************************************************/
static void ListModules (vlc_object_t *p_this, bool b_verbose)
{
    bool color = false;
    ShowConsole();
#ifndef _WIN32
    if (isatty(STDOUT_FILENO))
        color = var_InheritBool(p_this, "color");
#else
    (void) p_this;
#endif
    /* List all modules */
    size_t count;
    module_t **list = module_list_get (&count);
    /* Enumerate each module */
    for (size_t j = 0; j < count; j++)
    {
        module_t *p_parser = list[j];
        const char *objname = module_get_object (p_parser);
        printf(color ? GREEN"  %-22s "WHITE"%s\n"GRAY : "  %-22s %s\n",
               objname, module_gettext(p_parser, p_parser->psz_longname));
        if( b_verbose )
        {
            char *const *pp_shortcuts = p_parser->pp_shortcuts;
            for( unsigned i = 0; i < p_parser->i_shortcuts; i++ )
                if( strcmp( pp_shortcuts[i], objname ) )
                    printf(color ? CYAN"   s %s\n"GRAY : "   s %s\n",
                           pp_shortcuts[i]);
            if (p_parser->psz_capability != NULL)
                printf(color ? MAGENTA"   c %s (%d)\n"GRAY : "   c %s (%d)\n",
                       p_parser->psz_capability, p_parser->i_score);
        }
    }
    module_list_free (list);
    PauseConsole();
}
