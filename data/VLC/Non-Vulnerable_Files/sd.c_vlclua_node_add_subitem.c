}
static int vlclua_node_add_subitem( lua_State *L )
{
    services_discovery_t *p_sd = (services_discovery_t *)vlclua_get_this( L );
    input_item_t **pp_node = (input_item_t **)luaL_checkudata( L, 1, "node" );
    if( *pp_node )
    {
        if( lua_istable( L, -1 ) )
        {
            lua_getfield( L, -1, "path" );
            if( lua_isstring( L, -1 ) )
            {
                const char *psz_path = lua_tostring( L, -1 );
                /* The table must be at the top of the stack when calling
                 * vlclua_read_options() */
                char **ppsz_options = NULL;
                int i_options = 0;
                lua_pushvalue( L, -2 );
                vlclua_read_options( p_sd, L, &i_options, &ppsz_options );
                input_item_t *p_input = input_item_NewExt( psz_path,
                                                           psz_path, i_options,
                                                           (const char **)ppsz_options,
                                                           VLC_INPUT_OPTION_TRUSTED, -1 );
                lua_pop( L, 2 );
                if( p_input )
                {
                    input_item_node_t *p_input_node = input_item_node_Create( *pp_node );
                    vlclua_read_meta_data( p_sd, L, p_input );
                    /* This one is to be tested... */
                    vlclua_read_custom_meta_data( p_sd, L, p_input );
                    lua_getfield( L, -1, "duration" );
                    if( lua_isnumber( L, -1 ) )
                        input_item_SetDuration( p_input, (lua_tonumber( L, -1 )*1e6) );
                    else if( !lua_isnil( L, -1 ) )
                        msg_Warn( p_sd, "Item duration should be a number (in seconds)." );
                    lua_pop( L, 1 );
                    input_item_node_AppendItem( p_input_node, p_input );
                    input_item_node_PostAndDelete( p_input_node );
                    input_item_t **udata = (input_item_t **)
                                           lua_newuserdata( L, sizeof( input_item_t * ) );
                    *udata = p_input;
                    if( luaL_newmetatable( L, "input_item_t" ) )
                    {
                        lua_newtable( L );
                        luaL_register( L, NULL, vlclua_item_reg );
                        lua_setfield( L, -2, "__index" );
                        lua_pushliteral( L, "none of your business" );
                        lua_setfield( L, -2, "__metatable" );
                    }
                    lua_setmetatable( L, -2 );
                    vlc_gc_decref( p_input );
                }
                while( i_options > 0 )
                    free( ppsz_options[--i_options] );
                free( ppsz_options );
            }
            else
                msg_Err( p_sd, "node:add_subitem: the \"path\" parameter can't be empty" );
        }
        else
            msg_Err( p_sd, "Error parsing add_subitem arguments" );
    }
    return 1;
}
