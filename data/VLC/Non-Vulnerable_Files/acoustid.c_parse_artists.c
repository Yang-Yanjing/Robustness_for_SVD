}
static void parse_artists( json_value *node, musicbrainz_recording_t *record )
{
    /* take only main */
    if ( !node || node->type != json_array || node->u.array.length < 1 ) return;
    json_value *artistnode = node->u.array.values[ 0 ];
    json_value *value = jsongetbyname( artistnode, "name" );
    if ( value && value->type == json_string )
        record->psz_artist = strdup( value->u.string.ptr );
}
