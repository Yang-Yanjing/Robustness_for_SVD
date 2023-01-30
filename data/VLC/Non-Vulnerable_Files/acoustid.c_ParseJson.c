}
static bool ParseJson( vlc_object_t *p_obj, char *psz_buffer, acoustid_results_t *p_results )
{
    json_settings settings;
    char psz_error[128];
    memset (&settings, 0, sizeof (json_settings));
    json_value *root = json_parse_ex( &settings, psz_buffer, psz_error );
    if ( root == NULL )
    {
        msg_Warn( p_obj, "Can't parse json data: %s", psz_error );
        goto error;
    }
    if ( root->type != json_object )
    {
        msg_Warn( p_obj, "wrong json root node" );
        goto error;
    }
    json_value *node = jsongetbyname( root, "status" );
    if ( !node || node->type != json_string )
    {
        msg_Warn( p_obj, "status node not found or invalid" );
        goto error;
    }
    if ( strcmp( node->u.string.ptr, "ok" ) != 0 )
    {
        msg_Warn( p_obj, "Bad request status" );
        goto error;
    }
    node = jsongetbyname( root, "results" );
    if ( !node || node->type != json_array )
    {
        msg_Warn( p_obj, "Bad results array or no results" );
        goto error;
    }
    p_results->p_results = calloc( node->u.array.length, sizeof(acoustid_result_t) );
    if ( ! p_results->p_results ) goto error;
    p_results->count = node->u.array.length;
    for( unsigned int i=0; i<node->u.array.length; i++ )
    {
        json_value *resultnode = node->u.array.values[i];
        if ( resultnode && resultnode->type == json_object )
        {
            acoustid_result_t *p_result = & p_results->p_results[i];
            json_value *value = jsongetbyname( resultnode, "score" );
            if ( value && value->type == json_double )
                p_result->d_score = value->u.dbl;
            value = jsongetbyname( resultnode, "id" );
            if ( value && value->type == json_string )
                p_result->psz_id = strdup( value->u.string.ptr );
            parse_recordings( p_obj, jsongetbyname( resultnode, "recordings" ), p_result );
        }
    }
    json_value_free( root );
    return true;
error:
    if ( root ) json_value_free( root );
    return false;
}
