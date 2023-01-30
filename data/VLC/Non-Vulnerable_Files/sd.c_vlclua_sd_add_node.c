}
static int vlclua_sd_add_node( lua_State *L )
{
    services_discovery_t *p_sd = (services_discovery_t *)vlclua_get_this( L );
    if( lua_istable( L, -1 ) )
    {
        lua_getfield( L, -1, "title" );
        if( lua_isstring( L, -1 ) )
        {
            const char *psz_name = lua_tostring( L, -1 );
            input_item_t *p_input = input_item_NewWithType( "vlc://nop",
                                                            psz_name, 0, NULL, 0,
                                                            -1, ITEM_TYPE_NODE );
            lua_pop( L, 1 );
            if( p_input )
            {
                lua_getfield( L, -1, "arturl" );
                if( lua_isstring( L, -1 ) && strcmp( lua_tostring( L, -1 ), "" ) )
                {
                    char *psz_value = strdup( lua_tostring( L, -1 ) );
                    EnsureUTF8( psz_value );
                    msg_Dbg( p_sd, "ArtURL: %s", psz_value );
                    /** @todo Ask for art download if not local file */
                    input_item_SetArtURL( p_input, psz_value );
                    free( psz_value );
                }
                lua_pop( L, 1 );
                lua_getfield( L, -1, "category" );
                if( lua_isstring( L, -1 ) )
                    services_discovery_AddItem( p_sd, p_input, luaL_checkstring( L, -1 ) );
                else
                    services_discovery_AddItem( p_sd, p_input, NULL );
                input_item_t **udata = (input_item_t **)
                                       lua_newuserdata( L, sizeof( input_item_t * ) );
                *udata = p_input;
                if( luaL_newmetatable( L, "node" ) )
                {
                    lua_newtable( L );
                    luaL_register( L, NULL, vlclua_node_reg );
                    lua_setfield( L, -2, "__index" );
                }
                lua_setmetatable( L, -2 );
            }
        }
        else
            msg_Err( p_sd, "vlc.sd.add_node: the \"title\" parameter can't be empty" );
    }
    else
        msg_Err( p_sd, "Error parsing add_node arguments" );
    return 1;
}
