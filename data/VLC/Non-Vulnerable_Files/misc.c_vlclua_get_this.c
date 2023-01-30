}
vlc_object_t * vlclua_get_this( lua_State *L )
{
    return vlclua_get_object( L, vlclua_set_this );
}
