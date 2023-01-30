static int CacheSaveSubmodule (FILE *, const module_t *);
static int CacheSaveBank (FILE *file, const module_cache_t *cache,
                          size_t i_cache)
{
    uint32_t i_file_size = 0;
    /* Contains version number */
    if (fputs (CACHE_STRING, file) == EOF)
        goto error;
#ifdef DISTRO_VERSION
    /* Allow binary maintaner to pass a string to detect new binary version*/
    if (fputs( DISTRO_VERSION, file ) == EOF)
        goto error;
#endif
    /* Sub-version number (to avoid breakage in the dev version when cache
     * structure changes) */
    i_file_size = CACHE_SUBVERSION_NUM;
    if (fwrite (&i_file_size, sizeof (i_file_size), 1, file) != 1 )
        goto error;
    /* Header marker */
    i_file_size = ftell( file );
    if (fwrite (&i_file_size, sizeof (i_file_size), 1, file) != 1)
        goto error;
    if (fwrite( &i_cache, sizeof (i_cache), 1, file) != 1)
        goto error;
    for (unsigned i = 0; i < i_cache; i++)
    {
        module_t *module = cache[i].p_module;
        uint32_t i_submodule;
        /* Save additional infos */
        SAVE_STRING(module->psz_shortname);
        SAVE_STRING(module->psz_longname);
        SAVE_STRING(module->psz_help);
        SAVE_IMMEDIATE(module->i_shortcuts);
        for (unsigned j = 0; j < module->i_shortcuts; j++)
            SAVE_STRING(module->pp_shortcuts[j]);
        SAVE_STRING(module->psz_capability);
        SAVE_IMMEDIATE(module->i_score);
        SAVE_IMMEDIATE(module->b_unloadable);
        /* Config stuff */
        if (CacheSaveModuleConfig (file, module))
            goto error;
        SAVE_STRING(module->domain);
        i_submodule = module->submodule_count;
        SAVE_IMMEDIATE( i_submodule );
        if (CacheSaveSubmodule (file, module->submodule))
            goto error;
        /* Save common info */
        SAVE_STRING(cache[i].path);
        SAVE_IMMEDIATE(cache[i].mtime);
        SAVE_IMMEDIATE(cache[i].size);
    }
    if (fflush (file)) /* flush libc buffers */
        goto error;
    return 0; /* success! */
error:
    return -1;
}
