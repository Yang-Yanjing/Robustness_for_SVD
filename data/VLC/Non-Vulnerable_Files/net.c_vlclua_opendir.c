}
static int vlclua_opendir( lua_State *L )
{
    const char *psz_dir = luaL_checkstring( L, 1 );
    DIR *p_dir;
    int i = 0;
    if( ( p_dir = vlc_opendir( psz_dir ) ) == NULL )
        return luaL_error( L, "cannot open directory `%s'.", psz_dir );
    lua_newtable( L );
    for( ;; )
    {
        const char *psz_filename = vlc_readdir( p_dir );
        if( !psz_filename ) break;
        i++;
        lua_pushstring( L, psz_filename );
        lua_rawseti( L, -2, i );
    }
    closedir( p_dir );
    return 1;
}
