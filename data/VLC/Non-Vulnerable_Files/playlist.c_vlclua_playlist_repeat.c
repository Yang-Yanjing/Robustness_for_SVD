}
static int vlclua_playlist_repeat( lua_State * L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    int i_ret = vlclua_var_toggle_or_set( L, p_playlist, "repeat" );
    return i_ret;
}
