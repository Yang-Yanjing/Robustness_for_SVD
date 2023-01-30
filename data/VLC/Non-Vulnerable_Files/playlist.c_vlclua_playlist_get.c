}
static int vlclua_playlist_get( lua_State *L )
{
    playlist_t *p_playlist = vlclua_get_playlist_internal( L );
    PL_LOCK;
    playlist_item_t *p_item = NULL;
    if( lua_isnumber( L, 1 ) )
    {
        int i_id = lua_tointeger( L, 1 );
        p_item = playlist_ItemGetById( p_playlist, i_id );
        if( !p_item )
        {
            PL_UNLOCK;
            return 0; /* Should we return an error instead? */
        }
    }
    else if( lua_isstring( L, 1 ) )
    {
        const char *psz_what = lua_tostring( L, 1 );
        if( !strcasecmp( psz_what, "normal" )
         || !strcasecmp( psz_what, "playlist" ) )
            p_item = p_playlist->p_playing;
        else if( !strcasecmp( psz_what, "ml" )
              || !strcasecmp( psz_what, "media library" ) )
            p_item = p_playlist->p_media_library;
        else if( !strcasecmp( psz_what, "root" ) )
            p_item = p_playlist->p_root;
        else
        {
            /* currently, psz_what must be SD module's longname! */
            p_item = playlist_ChildSearchName( p_playlist->p_root, psz_what );
            if( !p_item )
            {
                PL_UNLOCK;
                return 0; /* Should we return an error instead? */
            }
        }
    }
    else
    {
        p_item = p_playlist->p_root;
    }
    push_playlist_item( L, p_item );
    PL_UNLOCK;
    return 1;
}
