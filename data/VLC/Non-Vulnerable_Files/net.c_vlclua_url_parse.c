 *****************************************************************************/
static int vlclua_url_parse( lua_State *L )
{
    const char *psz_url = luaL_checkstring( L, 1 );
    const char *psz_option = luaL_optstring( L, 2, NULL );
    vlc_url_t url;
    vlc_UrlParse( &url, psz_url, psz_option?*psz_option:0 );
    lua_newtable( L );
    lua_pushstring( L, url.psz_protocol );
    lua_setfield( L, -2, "protocol" );
    lua_pushstring( L, url.psz_username );
    lua_setfield( L, -2, "username" );
    lua_pushstring( L, url.psz_password );
    lua_setfield( L, -2, "password" );
    lua_pushstring( L, url.psz_host );
    lua_setfield( L, -2, "host" );
    lua_pushinteger( L, url.i_port );
    lua_setfield( L, -2, "port" );
    lua_pushstring( L, url.psz_path );
    lua_setfield( L, -2, "path" );
    lua_pushstring( L, url.psz_option );
    lua_setfield( L, -2, "option" );
    vlc_UrlClean( &url );
    return 1;
}
