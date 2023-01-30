}
static int vlclua_config_set( lua_State *L )
{
    vlc_object_t *p_this = vlclua_get_this( L );
    const char *psz_name = luaL_checkstring( L, 1 );
    switch( config_GetType( p_this, psz_name ) )
    {
        case VLC_VAR_STRING:
            config_PutPsz( p_this, psz_name, luaL_checkstring( L, 2 ) );
            break;
        case VLC_VAR_INTEGER:
            config_PutInt( p_this, psz_name, luaL_checkint( L, 2 ) );
            break;
        case VLC_VAR_BOOL:
            config_PutInt( p_this, psz_name, luaL_checkboolean( L, 2 ) );
            break;
        case VLC_VAR_FLOAT:
            config_PutFloat( p_this, psz_name,
                             luaL_checknumber( L, 2 ) );
            break;
        default:
            return vlclua_error( L );
    }
    return 0;
}
