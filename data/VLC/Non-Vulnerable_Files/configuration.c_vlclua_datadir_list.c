}
static int vlclua_datadir_list( lua_State *L )
{
    const char *psz_dirname = luaL_checkstring( L, 1 );
    char **ppsz_dir_list = NULL;
    int i = 1;
    if( vlclua_dir_list( psz_dirname, &ppsz_dir_list )
        != VLC_SUCCESS )
        return 0;
    lua_newtable( L );
    for( char **ppsz_dir = ppsz_dir_list; *ppsz_dir; ppsz_dir++ )
    {
        lua_pushstring( L, *ppsz_dir );
        lua_rawseti( L, -2, i );
        i ++;
    }
    vlclua_dir_list_free( ppsz_dir_list );
    return 1;
}
