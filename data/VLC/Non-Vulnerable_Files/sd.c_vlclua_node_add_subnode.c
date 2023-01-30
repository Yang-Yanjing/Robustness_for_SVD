}
static int vlclua_node_add_subnode( lua_State *L )
{
    services_discovery_t *p_sd = (services_discovery_t *)vlclua_get_this( L );
    input_item_t **pp_node = (input_item_t **)luaL_checkudata( L, 1, "node" );
    if( *pp_node )
    {
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
                    input_item_node_t *p_input_node = input_item_node_Create( *pp_node );
                    lua_getfield( L, -1, "arturl" );
                    if( lua_isstring( L, -1 ) && strcmp( lua_tostring( L, -1 ), "" ) )
                    {
                        char *psz_value = strdup( lua_tostring( L, -1 ) );
                        EnsureUTF8( psz_value );
                        msg_Dbg( p_sd, "ArtURL: %s", psz_value );
                        input_item_SetArtURL( p_input, psz_value );
                        free( psz_value );
                    }
                    input_item_node_AppendItem( p_input_node, p_input );
                    input_item_node_PostAndDelete( p_input_node );
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
                msg_Err( p_sd, "node:add_node: the \"title\" parameter can't be empty" );
        }
        else
            msg_Err( p_sd, "Error parsing add_node arguments" );
    }
    return 1;
}
