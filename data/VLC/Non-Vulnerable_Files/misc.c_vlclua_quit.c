 *****************************************************************************/
static int vlclua_quit( lua_State *L )
{
    vlc_object_t *p_this = vlclua_get_this( L );
    /* The rc.c code also stops the playlist ... not sure if this is needed
     * though. */
    libvlc_Quit( p_this->p_libvlc );
    return 0;
}
