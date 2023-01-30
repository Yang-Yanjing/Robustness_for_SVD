}
static void AddTrack( services_discovery_t *p_sd, LIBMTP_track_t *p_track )
{
    input_item_t *p_input;
    char *psz_string;
    char *extension;
    extension = rindex( p_track->filename, '.' );
    if( asprintf( &psz_string, "mtp://%"PRIu32":%"PRIu8":%"PRIu16":%d%s",
                  p_sd->p_sys->i_bus, p_sd->p_sys->i_dev,
                  p_sd->p_sys->i_product_id, p_track->item_id,
                  extension ) == -1 )
    {
        msg_Err( p_sd, "Error adding %s, skipping it", p_track->filename );
        return;
    }
    if( ( p_input = input_item_New( psz_string, p_track->title ) ) == NULL )
    {
        msg_Err( p_sd, "Error adding %s, skipping it", p_track->filename );
        free( psz_string );
        return;
    }
    free( psz_string );
    input_item_SetArtist( p_input, p_track->artist );
    input_item_SetGenre( p_input, p_track->genre );
    input_item_SetAlbum( p_input, p_track->album );
    if( asprintf( &psz_string, "%d", p_track->tracknumber ) != -1 )
    {
        input_item_SetTrackNum( p_input, psz_string );
        free( psz_string );
    }
    if( asprintf( &psz_string, "%d", p_track->rating ) != -1 )
    {
        input_item_SetRating( p_input, psz_string );
        free( psz_string );
    }
    input_item_SetDate( p_input, p_track->date );
    input_item_SetDuration( p_input, p_track->duration * 1000 );
    services_discovery_AddItem( p_sd, p_input, NULL );
    p_sd->p_sys->pp_items[p_sd->p_sys->i_count++] = p_input;
}
