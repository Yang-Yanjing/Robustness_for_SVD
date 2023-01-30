 *****************************************************************************/
static int ItemChange( vlc_object_t *p_this, const char *psz_var,
                       vlc_value_t oldval, vlc_value_t newval, void *param )
{
    VLC_UNUSED(psz_var); VLC_UNUSED(oldval); VLC_UNUSED(newval);
    char           psz_tmp[MAX_LENGTH];
    char           psz_notify[MAX_LENGTH];
    char           *psz_title;
    char           *psz_artist;
    char           *psz_album;
    char           *psz_arturl;
    input_thread_t *p_input = playlist_CurrentInput( (playlist_t*)p_this );
    intf_thread_t  *p_intf  = param;
    intf_sys_t     *p_sys   = p_intf->p_sys;
    if( !p_input )
        return VLC_SUCCESS;
    if( p_input->b_dead )
    {
        /* Not playing anything ... */
        vlc_object_release( p_input );
        return VLC_SUCCESS;
    }
    /* Wait a tad so the meta has been fetched
     * FIXME that's awfully wrong */
    msleep( 10000 );
    /* Playing something ... */
    input_item_t *p_input_item = input_GetItem( p_input );
    psz_title = input_item_GetTitleFbName( p_input_item );
    /* We need at least a title */
    if( EMPTY_STR( psz_title ) )
    {
        free( psz_title );
        vlc_object_release( p_input );
        return VLC_SUCCESS;
    }
    psz_artist = input_item_GetArtist( p_input_item );
    psz_album = input_item_GetAlbum( p_input_item );
    if( !EMPTY_STR( psz_artist ) )
    {
        if( !EMPTY_STR( psz_album ) )
            snprintf( psz_tmp, MAX_LENGTH, "<b>%s</b>\n%s\n[%s]",
                      psz_title, psz_artist, psz_album );
        else
            snprintf( psz_tmp, MAX_LENGTH, "<b>%s</b>\n%s",
                      psz_title, psz_artist );
    }
    else
        snprintf( psz_tmp, MAX_LENGTH, "<b>%s</b>", psz_title );
    free( psz_title );
    free( psz_artist );
    free( psz_album );
    GdkPixbuf *pix = NULL;
    psz_arturl = input_item_GetArtURL( p_input_item );
    vlc_object_release( p_input );
    if( psz_arturl )
    {
        char *psz = make_path( psz_arturl );
        free( psz_arturl );
        psz_arturl = psz;
    }
    if( psz_arturl )
    { /* scale the art to show it in notify popup */
        GError *p_error = NULL;
        pix = gdk_pixbuf_new_from_file_at_scale( psz_arturl,
                                                 72, 72, TRUE, &p_error );
    }
    else /* else we show state-of-the art logo */
    {
        /* First try to get an icon from the current theme. */
        GtkIconTheme* p_theme = gtk_icon_theme_get_default();
        pix = gtk_icon_theme_load_icon( p_theme, "vlc", 72, 0, NULL);
        if( !pix )
        {
        /* Load icon from share/ */
            GError *p_error = NULL;
            char *psz_pixbuf;
            char *psz_data = config_GetDataDir();
            if( asprintf( &psz_pixbuf, "%s/icons/48x48/vlc.png", psz_data ) >= 0 )
            {
                pix = gdk_pixbuf_new_from_file( psz_pixbuf, &p_error );
                free( psz_pixbuf );
            }
            free( psz_data );
        }
    }
    free( psz_arturl );
    /* we need to replace '&' with '&amp;' because '&' is a keyword of
     * notification-daemon parser */
    const int i_len = strlen( psz_tmp );
    int i_notify = 0;
    for( int i = 0; i < i_len && i_notify < ( MAX_LENGTH - 5 ); i++ )
    { /* we use MAX_LENGTH - 5 because if the last char of psz_tmp is '&'
       * we will need 5 more characters: 'amp;\0' .
       * however that's unlikely to happen because the last char is '\0' */
        if( psz_tmp[i] != '&' )
        {
            psz_notify[i_notify] = psz_tmp[i];
        }
        else
        {
            strcpy( &psz_notify[i_notify], "&amp;" );
            i_notify += 4;
        }
        i_notify++;
    }
    psz_notify[i_notify] = '\0';
    vlc_mutex_lock( &p_sys->lock );
    Notify( p_this, psz_notify, pix, p_intf );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
