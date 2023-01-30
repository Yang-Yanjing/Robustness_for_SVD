}
static int vlclua_sd_remove_node( lua_State *L )
{
    services_discovery_t *p_sd = (services_discovery_t *)vlclua_get_this( L );
    if( !lua_isnil( L, 1 ) )
    {
        input_item_t **pp_input = luaL_checkudata( L, 1, "node" );
        if( *pp_input )
            services_discovery_RemoveItem( p_sd, *pp_input );
        /* Make sure we won't try to remove it again */
        *pp_input = NULL;
    }
    return 1;
}
