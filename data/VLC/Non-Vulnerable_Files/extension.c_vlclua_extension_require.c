 **/
static int vlclua_extension_require( lua_State *L )
{
    const char *psz_module = luaL_checkstring( L, 1 );
    vlc_object_t *p_this = vlclua_get_this( L );
    extension_t *p_ext = vlclua_extension_get( L );
    msg_Dbg( p_this, "loading module '%s' from extension package",
             psz_module );
    char *psz_fullpath, *psz_package, *sep;
    psz_package = strdup( p_ext->psz_name );
    sep = strrchr( psz_package, '/' );
    if( !sep )
    {
        free( psz_package );
        return luaL_error( L, "could not find package name" );
    }
    *sep = '\0';
    if( -1 == asprintf( &psz_fullpath,
                        "%s/modules/%s.luac", psz_package, psz_module ) )
    {
        free( psz_package );
        return 1;
    }
    int i_ret = vlclua_dofile( p_this, L, psz_fullpath );
    if( i_ret != 0 )
    {
        // Remove trailing 'c' --> try with .lua script
        psz_fullpath[ strlen( psz_fullpath ) - 1 ] = '\0';
        i_ret = vlclua_dofile( p_this, L, psz_fullpath );
    }
    free( psz_fullpath );
    free( psz_package );
    if( i_ret != 0 )
    {
        return luaL_error( L, "unable to load module '%s' from package",
                           psz_module );
    }
    return 0;
}
