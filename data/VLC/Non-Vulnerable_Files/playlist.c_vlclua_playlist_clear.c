}
static int vlclua_playlist_clear( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    playlist_Stop( p_playlist ); /* Isn't this already implied by Clear? */
    playlist_Clear( p_playlist, pl_Unlocked );
    return 0;
}
