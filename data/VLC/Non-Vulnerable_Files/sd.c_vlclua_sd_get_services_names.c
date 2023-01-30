};
static int vlclua_sd_get_services_names( lua_State *L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    char **ppsz_longnames;
    char **ppsz_names = vlc_sd_GetNames( p_playlist, &ppsz_longnames, NULL );
    if( !ppsz_names )
        return 0;
    char **ppsz_longname = ppsz_longnames;
    char **ppsz_name = ppsz_names;
    lua_settop( L, 0 );
    lua_newtable( L );
    for( ; *ppsz_name; ppsz_name++,ppsz_longname++ )
    {
        lua_pushstring( L, *ppsz_longname );
        lua_setfield( L, -2, *ppsz_name );
        free( *ppsz_name );
        free( *ppsz_longname );
    }
    free( ppsz_names );
    free( ppsz_longnames );
    return 1;
}
