}
static int vlclua_userdatadir( lua_State *L )
{
    char *dir = config_GetUserDir( VLC_DATA_DIR );
    lua_pushstring( L, dir );
    free( dir );
    return 1;
}
