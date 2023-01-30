 *****************************************************************************/
static int vlclua_datadir( lua_State *L )
{
    char *psz_data = config_GetDataDir();
    lua_pushstring( L, psz_data );
    free( psz_data );
    return 1;
}
