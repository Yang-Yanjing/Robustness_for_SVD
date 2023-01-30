 */
static bool playlist_LiveSearchUpdateInternal( playlist_item_t *p_root,
                                               const char *psz_string, bool b_recursive )
{
    int i;
    bool b_match = false;
    for( i = 0 ; i < p_root->i_children ; i ++ )
    {
        bool b_enable = false;
        playlist_item_t *p_item = p_root->pp_children[i];
        // Go recurssively if their is some children
        if( b_recursive && p_item->i_children >= 0 &&
            playlist_LiveSearchUpdateInternal( p_item, psz_string, true ) )
        {
            b_enable = true;
        }
        if( !b_enable )
        {
            vlc_mutex_lock( &p_item->p_input->lock );
            // Do we have some meta ?
            if( p_item->p_input->p_meta )
            {
                // Use Title or fall back to psz_name
                const char *psz_title = vlc_meta_Get( p_item->p_input->p_meta, vlc_meta_Title );
                if( !psz_title )
                    psz_title = p_item->p_input->psz_name;
                const char *psz_album = vlc_meta_Get( p_item->p_input->p_meta, vlc_meta_Album );
                const char *psz_artist = vlc_meta_Get( p_item->p_input->p_meta, vlc_meta_Artist );
                b_enable = ( psz_title && vlc_strcasestr( psz_title, psz_string ) ) ||
                           ( psz_album && vlc_strcasestr( psz_album, psz_string ) ) ||
                           ( psz_artist && vlc_strcasestr( psz_artist, psz_string ) );
            }
            else
                b_enable = p_item->p_input->psz_name && vlc_strcasestr( p_item->p_input->psz_name, psz_string );
            vlc_mutex_unlock( &p_item->p_input->lock );
        }
        if( b_enable )
            p_item->i_flags &= ~PLAYLIST_DBL_FLAG;
        else
            p_item->i_flags |= PLAYLIST_DBL_FLAG;
        b_match |= b_enable;
   }
   return b_match;
}
