}
static int GetTracks( access_t *p_access, input_item_t *p_current )
{
    access_sys_t *p_sys = p_access->p_sys;
    const int i_titles = ioctl_GetTracksMap( VLC_OBJECT(p_access),
                                             p_sys->vcddev, &p_sys->p_sectors );
    if( i_titles <= 0 )
    {
        if( i_titles < 0 )
            msg_Err( p_access, "unable to count tracks" );
        else if( i_titles <= 0 )
            msg_Err( p_access, "no audio tracks found" );
        return VLC_EGENERIC;;
    }
    /* */
    input_item_SetName( p_current, "Audio CD" );
    const char *psz_album = NULL;
    const char *psz_year = NULL;
    const char *psz_genre = NULL;
    const char *psz_artist = NULL;
    const char *psz_description = NULL;
/* Return true if the given string is not NULL and not empty */
#define NONEMPTY( psz ) ( (psz) && *(psz) )
/* If the given string is NULL or empty, fill it by the return value of 'code' */
#define ON_EMPTY( psz, code ) do { if( !NONEMPTY( psz) ) { (psz) = code; } } while(0)
    /* Retreive CDDB information */
#ifdef HAVE_LIBCDDB
    char psz_year_buffer[4+1];
    msg_Dbg( p_access, "fetching infos with CDDB" );
    cddb_disc_t *p_disc = GetCDDBInfo( p_access, i_titles, p_sys->p_sectors );
    if( p_disc )
    {
        psz_album = cddb_disc_get_title( p_disc );
        psz_genre = cddb_disc_get_genre( p_disc );
        /* */
        const unsigned i_year = cddb_disc_get_year( p_disc );
        if( i_year > 0 )
        {
            psz_year = psz_year_buffer;
            snprintf( psz_year_buffer, sizeof(psz_year_buffer), "%u", i_year );
        }
        /* Set artist only if unique */
        for( int i = 0; i < i_titles; i++ )
        {
            cddb_track_t *t = cddb_disc_get_track( p_disc, i );
            if( !t )
                continue;
            const char *psz_track_artist = cddb_track_get_artist( t );
            if( psz_artist && psz_track_artist &&
                strcmp( psz_artist, psz_track_artist ) )
            {
                psz_artist = NULL;
                break;
            }
            psz_artist = psz_track_artist;
        }
    }
    else
        msg_Dbg( p_access, "GetCDDBInfo failed" );
#endif
    /* CD-Text */
    vlc_meta_t **pp_cd_text;
    int        i_cd_text;
    if( ioctl_GetCdText( VLC_OBJECT(p_access), p_sys->vcddev, &pp_cd_text, &i_cd_text ) )
    {
        msg_Dbg( p_access, "CD-TEXT information missing" );
        i_cd_text = 0;
        pp_cd_text = NULL;
    }
    /* Retrieve CD-TEXT information but prefer CDDB */
    if( i_cd_text > 0 && pp_cd_text[0] )
    {
        const vlc_meta_t *p_disc = pp_cd_text[0];
        ON_EMPTY( psz_album,       vlc_meta_Get( p_disc, vlc_meta_Album ) );
        ON_EMPTY( psz_genre,       vlc_meta_Get( p_disc, vlc_meta_Genre ) );
        ON_EMPTY( psz_artist,      vlc_meta_Get( p_disc, vlc_meta_Artist ) );
        ON_EMPTY( psz_description, vlc_meta_Get( p_disc, vlc_meta_Description ) );
    }
    if( NONEMPTY( psz_album ) )
    {
        input_item_SetName( p_current, psz_album );
        input_item_SetAlbum( p_current, psz_album );
    }
    if( NONEMPTY( psz_genre ) )
        input_item_SetGenre( p_current, psz_genre );
    if( NONEMPTY( psz_artist ) )
        input_item_SetArtist( p_current, psz_artist );
    if( NONEMPTY( psz_year ) )
        input_item_SetDate( p_current, psz_year );
    if( NONEMPTY( psz_description ) )
        input_item_SetDescription( p_current, psz_description );
    const mtime_t i_duration = (int64_t)( p_sys->p_sectors[i_titles] - p_sys->p_sectors[0] ) *
                               CDDA_DATA_SIZE * 1000000 / 44100 / 2 / 2;
    input_item_SetDuration( p_current, i_duration );
    input_item_node_t *p_root = input_item_node_Create( p_current );
    /* Build title table */
    for( int i = 0; i < i_titles; i++ )
    {
        char *psz_uri, *psz_opt, *psz_name;
        msg_Dbg( p_access, "track[%d] start=%d", i, p_sys->p_sectors[i] );
        if( asprintf( &psz_uri, "cdda://%s", p_access->psz_location ) == -1 )
            continue;
        /* Define a "default name" */
        if( asprintf( &psz_name, _("Audio CD - Track %02i"), (i+1) ) == -1 )
            psz_name = psz_uri;
        /* Create playlist items */
        const mtime_t i_duration = (int64_t)( p_sys->p_sectors[i+1] - p_sys->p_sectors[i] ) *
                                   CDDA_DATA_SIZE * 1000000 / 44100 / 2 / 2;
        input_item_t *p_item = input_item_NewWithType( psz_uri, psz_name, 0,
                                         NULL, 0, i_duration, ITEM_TYPE_DISC );
        if( likely(psz_name != psz_uri) )
            free( psz_name );
        free( psz_uri );
        if( unlikely(p_item == NULL) )
            continue;
        input_item_CopyOptions( p_current, p_item );
        if( likely(asprintf( &psz_opt, "cdda-track=%i", i+1 ) != -1) )
        {
            input_item_AddOption( p_item, psz_opt, VLC_INPUT_OPTION_TRUSTED );
            free( psz_opt );
        }
        if( likely(asprintf( &psz_opt, "cdda-first-sector=%i",
                             p_sys->p_sectors[i] ) != -1) )
        {
            input_item_AddOption( p_item, psz_opt, VLC_INPUT_OPTION_TRUSTED );
            free( psz_opt );
        }
        if( likely(asprintf( &psz_opt, "cdda-last-sector=%i",
                             p_sys->p_sectors[i+1] ) != -1) )
        {
            input_item_AddOption( p_item, psz_opt, VLC_INPUT_OPTION_TRUSTED );
            free( psz_opt );
        }
        const char *psz_track_title = NULL;
        const char *psz_track_artist = NULL;
        const char *psz_track_genre = NULL;
        const char *psz_track_description = NULL;
#ifdef HAVE_LIBCDDB
        /* Retreive CDDB information */
        if( p_disc )
        {
            cddb_track_t *t = cddb_disc_get_track( p_disc, i );
            if( t != NULL )
            {
                psz_track_title = cddb_track_get_title( t );
                psz_track_artist = cddb_track_get_artist( t );
            }
        }
#endif
        /* Retreive CD-TEXT information but prefer CDDB */
        if( i+1 < i_cd_text && pp_cd_text[i+1] )
        {
            const vlc_meta_t *t = pp_cd_text[i+1];
            ON_EMPTY( psz_track_title,       vlc_meta_Get( t, vlc_meta_Title ) );
            ON_EMPTY( psz_track_artist,      vlc_meta_Get( t, vlc_meta_Artist ) );
            ON_EMPTY( psz_track_genre,       vlc_meta_Get( t, vlc_meta_Genre ) );
            ON_EMPTY( psz_track_description, vlc_meta_Get( t, vlc_meta_Description ) );
        }
        /* */
        ON_EMPTY( psz_track_artist,       psz_artist );
        ON_EMPTY( psz_track_genre,        psz_genre );
        ON_EMPTY( psz_track_description,  psz_description );
        /* */
        if( NONEMPTY( psz_track_title ) )
        {
            input_item_SetName( p_item, psz_track_title );
            input_item_SetTitle( p_item, psz_track_title );
        }
        if( NONEMPTY( psz_track_artist ) )
            input_item_SetArtist( p_item, psz_track_artist );
        if( NONEMPTY( psz_track_genre ) )
            input_item_SetGenre( p_item, psz_track_genre );
        if( NONEMPTY( psz_track_description ) )
            input_item_SetDescription( p_item, psz_track_description );
        if( NONEMPTY( psz_album ) )
            input_item_SetAlbum( p_item, psz_album );
        if( NONEMPTY( psz_year ) )
            input_item_SetDate( p_item, psz_year );
        char psz_num[3+1];
        snprintf( psz_num, sizeof(psz_num), "%d", 1+i );
        input_item_SetTrackNum( p_item, psz_num );
        input_item_node_AppendItem( p_root, p_item );
        vlc_gc_decref( p_item );
    }
#undef ON_EMPTY
#undef NONEMPTY
    input_item_node_PostAndDelete( p_root );
    /* */
    for( int i = 0; i < i_cd_text; i++ )
    {
        vlc_meta_t *p_meta = pp_cd_text[i];
        if( !p_meta )
            continue;
        vlc_meta_Delete( p_meta );
    }
    free( pp_cd_text );
#ifdef HAVE_LIBCDDB
    if( p_disc )
        cddb_disc_destroy( p_disc );
#endif
    return VLC_SUCCESS;
}
