}
static int vlclua_playlist_status( lua_State *L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    switch( playlist_Status( p_playlist ) )
    {
        case PLAYLIST_STOPPED:
            lua_pushliteral( L, "stopped" );
            break;
        case PLAYLIST_RUNNING:
            lua_pushliteral( L, "playing" );
            break;
        case PLAYLIST_PAUSED:
            lua_pushliteral( L, "paused" );
            break;
        default:
            lua_pushliteral( L, "unknown" );
            break;
    }
    PL_UNLOCK;
    return 1;
}
