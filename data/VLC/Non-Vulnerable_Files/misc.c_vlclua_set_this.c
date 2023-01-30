#undef vlclua_set_this
void vlclua_set_this( lua_State *L, vlc_object_t *p_this )
{
    vlclua_set_object( L, vlclua_set_this, p_this );
}
