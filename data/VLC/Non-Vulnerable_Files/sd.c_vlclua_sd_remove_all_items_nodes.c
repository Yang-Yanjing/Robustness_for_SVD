}
static int vlclua_sd_remove_all_items_nodes( lua_State *L )
{
    services_discovery_t *p_sd = (services_discovery_t *)vlclua_get_this( L );
    services_discovery_RemoveAll( p_sd );
    return 1;
}
