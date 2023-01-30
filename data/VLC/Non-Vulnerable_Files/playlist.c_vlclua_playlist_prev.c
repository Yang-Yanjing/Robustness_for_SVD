}
static int vlclua_playlist_prev( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Prev( p_playlist );
    return 0;
}
