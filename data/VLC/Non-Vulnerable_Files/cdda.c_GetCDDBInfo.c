#ifdef HAVE_LIBCDDB
static cddb_disc_t *GetCDDBInfo( access_t *p_access, int i_titles, int *p_sectors )
{
    if( var_InheritInteger( p_access, "album-art" ) != ALBUM_ART_ALL &&
        !  var_InheritBool( p_access, "metadata-network-access" ) )
    {
        msg_Dbg( p_access, "Album art policy set to manual; no automatic fetching" );
        return NULL;
    }
    /* */
    cddb_conn_t *p_cddb = cddb_new();
    if( !p_cddb )
    {
        msg_Warn( p_access, "unable to use CDDB" );
        return NULL;
    }
    /* */
    cddb_http_enable( p_cddb );
    char *psz_tmp = var_InheritString( p_access, "cddb-server" );
    if( psz_tmp )
    {
        cddb_set_server_name( p_cddb, psz_tmp );
        free( psz_tmp );
    }
    cddb_set_server_port( p_cddb, var_InheritInteger( p_access, "cddb-port" ) );
    cddb_set_email_address( p_cddb, "vlc@videolan.org" );
    cddb_set_http_path_query( p_cddb, "/~cddb/cddb.cgi" );
    cddb_set_http_path_submit( p_cddb, "/~cddb/submit.cgi" );
    char *psz_cachedir;
    char *psz_temp = config_GetUserDir( VLC_CACHE_DIR );
    if( asprintf( &psz_cachedir, "%s" DIR_SEP "cddb", psz_temp ) > 0 ) {
        cddb_cache_enable( p_cddb );
        cddb_cache_set_dir( p_cddb, psz_cachedir );
        free( psz_cachedir );
    }
    free( psz_temp );
    cddb_set_timeout( p_cddb, 10 );
    /* */
    cddb_disc_t *p_disc = cddb_disc_new();
    if( !p_disc )
    {
        msg_Err( p_access, "unable to create CDDB disc structure." );
        goto error;
    }
    int64_t i_length = 2000000; /* PreGap */
    for( int i = 0; i < i_titles; i++ )
    {
        cddb_track_t *t = cddb_track_new();
        cddb_track_set_frame_offset( t, p_sectors[i] + 150 );  /* Pregap offset */
        cddb_disc_add_track( p_disc, t );
        const int64_t i_size = ( p_sectors[i+1] - p_sectors[i] ) *
                               (int64_t)CDDA_DATA_SIZE;
        i_length += INT64_C(1000000) * i_size / 44100 / 4  ;
    }
    cddb_disc_set_length( p_disc, (int)(i_length/1000000) );
    if( !cddb_disc_calc_discid( p_disc ) )
    {
        msg_Err( p_access, "CDDB disc ID calculation failed" );
        goto error;
    }
    const int i_matches = cddb_query( p_cddb, p_disc );
    if( i_matches < 0 )
    {
        msg_Warn( p_access, "CDDB error: %s", cddb_error_str(errno) );
        goto error;
    }
    else if( i_matches == 0 )
    {
        msg_Dbg( p_access, "Couldn't find any matches in CDDB." );
        goto error;
    }
    else if( i_matches > 1 )
        msg_Warn( p_access, "found %d matches in CDDB. Using first one.", i_matches );
    cddb_read( p_cddb, p_disc );
    cddb_destroy( p_cddb);
    return p_disc;
error:
    if( p_disc )
        cddb_disc_destroy( p_disc );
    cddb_destroy( p_cddb );
    return NULL;
}
