 */
playlist_t *playlist_Create( vlc_object_t *p_parent )
{
    playlist_t *p_playlist;
    playlist_private_t *p;
    /* Allocate structure */
    p = vlc_custom_create( p_parent, sizeof( *p ), "playlist" );
    if( !p )
        return NULL;
    assert( offsetof( playlist_private_t, public_data ) == 0 );
    p_playlist = &p->public_data;
    TAB_INIT( pl_priv(p_playlist)->i_sds, pl_priv(p_playlist)->pp_sds );
    VariablesInit( p_playlist );
    vlc_mutex_init( &p->lock );
    vlc_cond_init( &p->signal );
    p->killed = false;
    /* Initialise data structures */
    pl_priv(p_playlist)->i_last_playlist_id = 0;
    pl_priv(p_playlist)->p_input = NULL;
    ARRAY_INIT( p_playlist->items );
    ARRAY_INIT( p_playlist->all_items );
    ARRAY_INIT( pl_priv(p_playlist)->items_to_delete );
    ARRAY_INIT( p_playlist->current );
    p_playlist->i_current_index = 0;
    pl_priv(p_playlist)->b_reset_currently_playing = true;
    pl_priv(p_playlist)->b_tree = var_InheritBool( p_parent, "playlist-tree" );
    pl_priv(p_playlist)->b_doing_ml = false;
    /* Create the root node */
    PL_LOCK;
    p_playlist->p_root = playlist_NodeCreate( p_playlist, NULL, NULL,
                                    PLAYLIST_END, 0, NULL );
    PL_UNLOCK;
    if( !p_playlist->p_root ) return NULL;
    /* Create currently playing items node */
    PL_LOCK;
    p_playlist->p_playing = playlist_NodeCreate(
        p_playlist, _( "Playlist" ), p_playlist->p_root,
        PLAYLIST_END, PLAYLIST_RO_FLAG, NULL );
    PL_UNLOCK;
    if( !p_playlist->p_playing ) return NULL;
    /* Create media library node */
    const bool b_ml = var_InheritBool( p_parent, "media-library");
    if( b_ml )
    {
        PL_LOCK;
        p_playlist->p_media_library = playlist_NodeCreate(
            p_playlist, _( "Media Library" ), p_playlist->p_root,
            PLAYLIST_END, PLAYLIST_RO_FLAG, NULL );
        PL_UNLOCK;
    }
    else
    {
        p_playlist->p_media_library = NULL;
    }
    p_playlist->p_root_category = p_playlist->p_root;
    p_playlist->p_root_onelevel = p_playlist->p_root;
    p_playlist->p_local_category = p_playlist->p_playing;
    p_playlist->p_local_onelevel = p_playlist->p_playing;
    p_playlist->p_ml_category = p_playlist->p_media_library;
    p_playlist->p_ml_onelevel = p_playlist->p_media_library;;
    /* Initial status */
    pl_priv(p_playlist)->status.p_item = NULL;
    pl_priv(p_playlist)->status.p_node = p_playlist->p_playing;
    pl_priv(p_playlist)->request.b_request = false;
    pl_priv(p_playlist)->status.i_status = PLAYLIST_STOPPED;
    if(b_ml)
        playlist_MLLoad( p_playlist );
    /* Preparser (and meta retriever) _after_ the Media Library*/
    if( var_InheritBool( p_parent, "auto-preparse" ) )
    {
        p->p_preparser = playlist_preparser_New( VLC_OBJECT(p_playlist) );
        if( unlikely(p->p_preparser == NULL) )
            msg_Err( p_playlist, "cannot create preparser" );
    }
    /* Input resources */
    p->p_input_resource = input_resource_New( VLC_OBJECT( p_playlist ) );
    if( unlikely(p->p_input_resource == NULL) )
        abort();
    /* Audio output (needed for volume and device controls). */
    audio_output_t *aout = input_resource_GetAout( p->p_input_resource );
    if( aout != NULL )
        input_resource_PutAout( p->p_input_resource, aout );
    /* Thread */
    playlist_Activate (p_playlist);
    /* Add service discovery modules */
    char *mods = var_InheritString( p_playlist, "services-discovery" );
    if( mods != NULL )
    {
        char *p = mods, *m;
        while( (m = strsep( &p, " :," )) != NULL )
            playlist_ServicesDiscoveryAdd( p_playlist, m );
        free( mods );
    }
    return p_playlist;
}
