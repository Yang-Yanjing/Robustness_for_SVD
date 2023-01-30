};
static void luaopen_net_intf( lua_State *L )
{
    lua_newtable( L );
    luaL_register( L, NULL, vlclua_net_intf_reg );
#define ADD_CONSTANT( value )    \
    lua_pushinteger( L, POLL##value ); \
    lua_setfield( L, -2, "POLL"#value );
    ADD_CONSTANT( IN )
    ADD_CONSTANT( PRI )
    ADD_CONSTANT( OUT )
    ADD_CONSTANT( ERR )
    ADD_CONSTANT( HUP )
    ADD_CONSTANT( NVAL )
    lua_setfield( L, -2, "net" );
}
