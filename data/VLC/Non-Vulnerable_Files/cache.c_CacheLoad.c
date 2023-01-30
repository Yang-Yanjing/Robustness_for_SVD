 */
size_t CacheLoad( vlc_object_t *p_this, const char *dir, module_cache_t **r )
{
    char *psz_filename;
    FILE *file;
    int i_size, i_read;
    char p_cachestring[sizeof(CACHE_STRING)];
    size_t i_cache;
    int32_t i_marker;
    assert( dir != NULL );
    *r = NULL;
    if( asprintf( &psz_filename, "%s"DIR_SEP CACHE_NAME, dir ) == -1 )
        return 0;
    msg_Dbg( p_this, "loading plugins cache file %s", psz_filename );
    file = vlc_fopen( psz_filename, "rb" );
    if( !file )
    {
        msg_Warn( p_this, "cannot read %s: %s", psz_filename,
                  vlc_strerror_c(errno) );
        free( psz_filename );
        return 0;
    }
    free( psz_filename );
    /* Check the file is a plugins cache */
    i_size = sizeof(CACHE_STRING) - 1;
    i_read = fread( p_cachestring, 1, i_size, file );
    if( i_read != i_size ||
        memcmp( p_cachestring, CACHE_STRING, i_size ) )
    {
        msg_Warn( p_this, "This doesn't look like a valid plugins cache" );
        fclose( file );
        return 0;
    }
#ifdef DISTRO_VERSION
    /* Check for distribution specific version */
    char p_distrostring[sizeof( DISTRO_VERSION )];
    i_size = sizeof( DISTRO_VERSION ) - 1;
    i_read = fread( p_distrostring, 1, i_size, file );
    if( i_read != i_size ||
        memcmp( p_distrostring, DISTRO_VERSION, i_size ) )
    {
        msg_Warn( p_this, "This doesn't look like a valid plugins cache" );
        fclose( file );
        return 0;
    }
#endif
    /* Check Sub-version number */
    i_read = fread( &i_marker, 1, sizeof(i_marker), file );
    if( i_read != sizeof(i_marker) || i_marker != CACHE_SUBVERSION_NUM )
    {
        msg_Warn( p_this, "This doesn't look like a valid plugins cache "
                  "(corrupted header)" );
        fclose( file );
        return 0;
    }
    /* Check header marker */
    i_read = fread( &i_marker, 1, sizeof(i_marker), file );
    if( i_read != sizeof(i_marker) ||
        i_marker != ftell( file ) - (int)sizeof(i_marker) )
    {
        msg_Warn( p_this, "This doesn't look like a valid plugins cache "
                  "(corrupted header)" );
        fclose( file );
        return 0;
    }
    if (fread( &i_cache, 1, sizeof(i_cache), file ) != sizeof(i_cache) )
    {
        msg_Warn( p_this, "This doesn't look like a valid plugins cache "
                  "(file too short)" );
        fclose( file );
        return 0;
    }
    module_cache_t *cache = NULL;
    for (size_t count = 0; count < i_cache;)
    {
        module_t *module;
        int i_submodules;
        module = vlc_module_create (NULL);
        /* Load additional infos */
        LOAD_STRING(module->psz_shortname);
        LOAD_STRING(module->psz_longname);
        LOAD_STRING(module->psz_help);
        LOAD_IMMEDIATE(module->i_shortcuts);
        if (module->i_shortcuts > MODULE_SHORTCUT_MAX)
            goto error;
        else
        {
            module->pp_shortcuts =
                              xmalloc (sizeof (*module->pp_shortcuts) * module->i_shortcuts);
            for (unsigned j = 0; j < module->i_shortcuts; j++)
                LOAD_STRING(module->pp_shortcuts[j]);
        }
        LOAD_STRING(module->psz_capability);
        LOAD_IMMEDIATE(module->i_score);
        LOAD_IMMEDIATE(module->b_unloadable);
        /* Config stuff */
        if (CacheLoadModuleConfig (module, file) != VLC_SUCCESS)
            goto error;
        LOAD_STRING(module->domain);
        if (module->domain != NULL)
            vlc_bindtextdomain (module->domain);
        LOAD_IMMEDIATE( i_submodules );
        while( i_submodules-- )
        {
            module_t *submodule = vlc_module_create (module);
            free (submodule->pp_shortcuts);
            LOAD_STRING(submodule->psz_shortname);
            LOAD_STRING(submodule->psz_longname);
            LOAD_IMMEDIATE(submodule->i_shortcuts);
            if (submodule->i_shortcuts > MODULE_SHORTCUT_MAX)
                goto error;
            else
            {
                submodule->pp_shortcuts =
                           xmalloc (sizeof (*submodule->pp_shortcuts) * submodule->i_shortcuts);
                for (unsigned j = 0; j < submodule->i_shortcuts; j++)
                    LOAD_STRING(submodule->pp_shortcuts[j]);
            }
            LOAD_STRING(submodule->psz_capability);
            LOAD_IMMEDIATE(submodule->i_score);
        }
        char *path;
        struct stat st;
        /* Load common info */
        LOAD_STRING(path);
        if (path == NULL)
            goto error;
        LOAD_IMMEDIATE(st.st_mtime);
        LOAD_IMMEDIATE(st.st_size);
        CacheAdd (&cache, &count, path, &st, module);
        free (path);
        /* TODO: deal with errors */
    }
    fclose( file );
    *r = cache;
    return i_cache;
error:
    msg_Warn( p_this, "plugins cache not loaded (corrupted)" );
    /* TODO: cleanup */
    fclose( file );
    return 0;
}
