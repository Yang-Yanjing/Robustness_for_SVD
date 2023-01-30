}
static void parse_recordings( vlc_object_t *p_obj, json_value *node, acoustid_result_t *p_result )
{
    if ( !node || node->type != json_array ) return;
    p_result->recordings.p_recordings = calloc( node->u.array.length, sizeof(musicbrainz_recording_t) );
    if ( ! p_result->recordings.p_recordings ) return;
    p_result->recordings.count = node->u.array.length;
    for( unsigned int i=0; i<node->u.array.length; i++ )
    {
        musicbrainz_recording_t *record = & p_result->recordings.p_recordings[ i ];
        json_value *recordnode = node->u.array.values[ i ];
        if ( !recordnode || recordnode->type != json_object ) break;
        json_value *value = jsongetbyname( recordnode, "title" );
        if ( value && value->type == json_string )
            record->psz_title = strdup( value->u.string.ptr );
        value = jsongetbyname( recordnode, "id" );
        if ( value && value->type == json_string )
            strncpy( record->sz_musicbrainz_id, value->u.string.ptr, MB_ID_SIZE );
        parse_artists( jsongetbyname( recordnode, "artists" ), record );
        msg_Dbg( p_obj, "recording %d title %s %36s %s", i, record->psz_title, record->sz_musicbrainz_id, record->psz_artist );
    }
}
