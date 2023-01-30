}
static int Playlist( vlc_object_t *p_this, char const *psz_cmd,
                     vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    vlc_value_t val;
    intf_thread_t *p_intf = (intf_thread_t*)p_this;
    playlist_t *p_playlist = p_intf->p_sys->p_playlist;
    input_thread_t * p_input = playlist_CurrentInput( p_playlist );
    if( p_input )
    {
        int state = var_GetInteger( p_input, "state" );
        vlc_object_release( p_input );
        if( state == PAUSE_S )
        {
            msg_rc( "%s", _("Type 'pause' to continue.") );
            return VLC_EGENERIC;
        }
    }
    /* Parse commands that require a playlist */
    if( !strcmp( psz_cmd, "prev" ) )
    {
        playlist_Prev( p_playlist );
    }
    else if( !strcmp( psz_cmd, "next" ) )
    {
        playlist_Next( p_playlist );
    }
    else if( !strcmp( psz_cmd, "play" ) )
    {
        msg_Warn( p_playlist, "play" );
        playlist_Play( p_playlist );
    }
    else if( !strcmp( psz_cmd, "repeat" ) )
    {
        bool b_update = true;
        var_Get( p_playlist, "repeat", &val );
        if( strlen( newval.psz_string ) > 0 )
        {
            if ( ( !strncmp( newval.psz_string, "on", 2 )  &&  val.b_bool ) ||
                 ( !strncmp( newval.psz_string, "off", 3 ) && !val.b_bool ) )
            {
                b_update = false;
            }
        }
        if ( b_update )
        {
            val.b_bool = !val.b_bool;
            var_Set( p_playlist, "repeat", val );
        }
        msg_rc( "Setting repeat to %d", val.b_bool );
    }
    else if( !strcmp( psz_cmd, "loop" ) )
    {
        bool b_update = true;
        var_Get( p_playlist, "loop", &val );
        if( strlen( newval.psz_string ) > 0 )
        {
            if ( ( !strncmp( newval.psz_string, "on", 2 )  &&  val.b_bool ) ||
                 ( !strncmp( newval.psz_string, "off", 3 ) && !val.b_bool ) )
            {
                b_update = false;
            }
        }
        if ( b_update )
        {
            val.b_bool = !val.b_bool;
            var_Set( p_playlist, "loop", val );
        }
        msg_rc( "Setting loop to %d", val.b_bool );
    }
    else if( !strcmp( psz_cmd, "random" ) )
    {
        bool b_update = true;
        var_Get( p_playlist, "random", &val );
        if( strlen( newval.psz_string ) > 0 )
        {
            if ( ( !strncmp( newval.psz_string, "on", 2 )  &&  val.b_bool ) ||
                 ( !strncmp( newval.psz_string, "off", 3 ) && !val.b_bool ) )
            {
                b_update = false;
            }
        }
        if ( b_update )
        {
            val.b_bool = !val.b_bool;
            var_Set( p_playlist, "random", val );
        }
        msg_rc( "Setting random to %d", val.b_bool );
    }
    else if (!strcmp( psz_cmd, "goto" ) )
    {
        PL_LOCK;
        unsigned i_pos = atoi( newval.psz_string );
        unsigned i_size = p_playlist->items.i_size;
        if( i_pos <= 0 )
            msg_rc( "%s", _("Error: `goto' needs an argument greater than zero.") );
        else if( i_pos <= i_size )
        {
            playlist_item_t *p_item, *p_parent;
            p_item = p_parent = p_playlist->items.p_elems[i_pos-1];
            while( p_parent->p_parent )
                p_parent = p_parent->p_parent;
            playlist_Control( p_playlist, PLAYLIST_VIEWPLAY, pl_Locked,
                    p_parent, p_item );
        }
        else
            msg_rc( vlc_ngettext("Playlist has only %u element",
                                 "Playlist has only %u elements", i_size),
                     i_size );
        PL_UNLOCK;
    }
    else if( !strcmp( psz_cmd, "stop" ) )
    {
        playlist_Stop( p_playlist );
    }
    else if( !strcmp( psz_cmd, "clear" ) )
    {
        playlist_Stop( p_playlist );
        playlist_Clear( p_playlist, pl_Unlocked );
    }
    else if( !strcmp( psz_cmd, "add" ) &&
             newval.psz_string && *newval.psz_string )
    {
        input_item_t *p_item = parse_MRL( newval.psz_string );
        if( p_item )
        {
            msg_rc( "Trying to add %s to playlist.", newval.psz_string );
            int i_ret =playlist_AddInput( p_playlist, p_item,
                     PLAYLIST_GO|PLAYLIST_APPEND, PLAYLIST_END, true,
                     pl_Unlocked );
            vlc_gc_decref( p_item );
            if( i_ret != VLC_SUCCESS )
            {
                return VLC_EGENERIC;
            }
        }
    }
    else if( !strcmp( psz_cmd, "enqueue" ) &&
             newval.psz_string && *newval.psz_string )
    {
        input_item_t *p_item = parse_MRL( newval.psz_string );
        if( p_item )
        {
            msg_rc( "trying to enqueue %s to playlist", newval.psz_string );
            if( playlist_AddInput( p_playlist, p_item,
                               PLAYLIST_APPEND, PLAYLIST_END, true,
                               pl_Unlocked ) != VLC_SUCCESS )
            {
                return VLC_EGENERIC;
            }
        }
    }
    else if( !strcmp( psz_cmd, "playlist" ) )
    {
        msg_rc( "+----[ Playlist ]" );
        print_playlist( p_intf, p_playlist->p_root_category, 0 );
        msg_rc( "+----[ End of playlist ]" );
    }
    else if( !strcmp( psz_cmd, "sort" ))
    {
        PL_LOCK;
        playlist_RecursiveNodeSort( p_playlist, p_playlist->p_root_onelevel,
                                    SORT_ARTIST, ORDER_NORMAL );
        PL_UNLOCK;
    }
    else if( !strcmp( psz_cmd, "status" ) )
    {
        input_thread_t * p_input = playlist_CurrentInput( p_playlist );
        if( p_input )
        {
            /* Replay the current state of the system. */
            char *psz_uri =
                    input_item_GetURI( input_GetItem( p_input ) );
            vlc_object_release( p_input );
            if( likely(psz_uri != NULL) )
            {
                msg_rc( STATUS_CHANGE "( new input: %s )", psz_uri );
                free( psz_uri );
            }
        }
        float volume = playlist_VolumeGet( p_playlist );
        if( volume >= 0.f )
            msg_rc( STATUS_CHANGE "( audio volume: %ld )",
                    lroundf(volume * AOUT_VOLUME_DEFAULT) );
        int status;
        PL_LOCK;
        status = playlist_Status(p_playlist);
        PL_UNLOCK;
        switch( status )
        {
            case PLAYLIST_STOPPED:
                msg_rc( STATUS_CHANGE "( stop state: 5 )" );
                break;
            case PLAYLIST_RUNNING:
                msg_rc( STATUS_CHANGE "( play state: 3 )" );
                break;
            case PLAYLIST_PAUSED:
                msg_rc( STATUS_CHANGE "( pause state: 4 )" );
                break;
            default:
                msg_rc( STATUS_CHANGE "( unknown state: -1 )" );
                break;
        }
    }
    /*
     * sanity check
     */
    else
    {
        msg_rc( "unknown command!" );
    }
    return VLC_SUCCESS;
}
