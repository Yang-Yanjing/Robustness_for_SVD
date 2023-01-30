 **/
int vlclua_extension_deactivate( lua_State *L )
{
    extension_t *p_ext = vlclua_extension_get( L );
    int i_ret = Deactivate( p_ext->p_sys->p_mgr, p_ext );
    return ( i_ret == VLC_SUCCESS ) ? 1 : 0;
}
