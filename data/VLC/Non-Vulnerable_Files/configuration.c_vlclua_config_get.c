 *****************************************************************************/
static int vlclua_config_get( lua_State *L )
{
    vlc_object_t * p_this = vlclua_get_this( L );
    const char *psz_name = luaL_checkstring( L, 1 );
    switch( config_GetType( p_this, psz_name ) )
    {
        case VLC_VAR_STRING:
        {
            char *psz = config_GetPsz( p_this, psz_name );
            lua_pushstring( L, psz );
            free( psz );
            break;
        }
        case VLC_VAR_INTEGER:
            lua_pushinteger( L, config_GetInt( p_this, psz_name ) );
            break;
        case VLC_VAR_BOOL:
            lua_pushboolean( L, config_GetInt( p_this, psz_name ) );
            break;
        case VLC_VAR_FLOAT:
            lua_pushnumber( L, config_GetFloat( p_this, psz_name ) );
            break;
        default:
            return vlclua_error( L );
    }
    return 1;
}
