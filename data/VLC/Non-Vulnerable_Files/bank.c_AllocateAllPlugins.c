 */
static void AllocateAllPlugins (vlc_object_t *p_this)
{
    char *paths;
    cache_mode_t mode;
    if( !var_InheritBool( p_this, "plugins-cache" ) )
        mode = CACHE_IGNORE;
    else if( var_InheritBool( p_this, "reset-plugins-cache" ) )
        mode = CACHE_RESET;
    else
        mode = CACHE_USE;
#if VLC_WINSTORE_APP
    /* Windows Store Apps can not load external plugins with absolute paths. */
    AllocatePluginPath (p_this, "plugins", mode);
#else
    /* Contruct the special search path for system that have a relocatable
     * executable. Set it to <vlc path>/plugins. */
    char *vlcpath = config_GetLibDir ();
    if (likely(vlcpath != NULL)
     && likely(asprintf (&paths, "%s" DIR_SEP "plugins", vlcpath) != -1))
    {
        AllocatePluginPath (p_this, paths, mode);
        free( paths );
    }
    free (vlcpath);
#endif /* VLC_WINSTORE_APP */
    /* If the user provided a plugin path, we add it to the list */
    paths = getenv( "VLC_PLUGIN_PATH" );
    if( paths == NULL )
        return;
    paths = strdup( paths ); /* don't harm the environment ! :) */
    if( unlikely(paths == NULL) )
        return;
    for( char *buf, *path = strtok_r( paths, PATH_SEP, &buf );
         path != NULL;
         path = strtok_r( NULL, PATH_SEP, &buf ) )
        AllocatePluginPath (p_this, path, mode);
    free( paths );
}
