}
static void push_playlist_item( lua_State *L, playlist_item_t *p_item )
{
    input_item_t *p_input = p_item->p_input;
    int i_flags = 0;
    i_flags = p_item->i_flags;
    lua_newtable( L );
    lua_pushinteger( L, p_item->i_id );
    lua_setfield( L, -2, "id" );
    lua_newtable( L );
#define CHECK_AND_SET_FLAG( name, label ) \
    if( i_flags & PLAYLIST_ ## name ## _FLAG ) \
    { \
        lua_pushboolean( L, 1 ); \
        lua_setfield( L, -2, #label ); \
    }
    CHECK_AND_SET_FLAG( SAVE, save )
    CHECK_AND_SET_FLAG( SKIP, skip )
    CHECK_AND_SET_FLAG( DBL, disabled )
    CHECK_AND_SET_FLAG( RO, ro )
    CHECK_AND_SET_FLAG( REMOVE, remove )
    CHECK_AND_SET_FLAG( EXPANDED, expanded )
#undef CHECK_AND_SET_FLAG
    lua_setfield( L, -2, "flags" );
    if( p_input )
    {
        char *psz_name = input_item_GetTitleFbName( p_input );
        lua_pushstring( L, psz_name );
        free( psz_name );
        lua_setfield( L, -2, "name" );
        lua_pushstring( L, p_input->psz_uri );
        lua_setfield( L, -2, "path" );
        if( p_input->i_duration < 0 )
            lua_pushnumber( L, -1 );
        else
            lua_pushnumber( L, ((double)p_input->i_duration)*1e-6 );
        lua_setfield( L, -2, "duration" );
        lua_pushinteger( L, p_input->i_nb_played );
        lua_setfield( L, -2, "nb_played" );
        luaopen_input_item( L, p_input );
        /* TODO: add (optional) info categories, meta, options, es */
    }
    if( p_item->i_children >= 0 )
    {
        int i;
        lua_createtable( L, p_item->i_children, 0 );
        for( i = 0; i < p_item->i_children; i++ )
        {
            push_playlist_item( L, p_item->pp_children[i] );
            lua_rawseti( L, -2, i+1 );
        }
        lua_setfield( L, -2, "children" );
    }
}
