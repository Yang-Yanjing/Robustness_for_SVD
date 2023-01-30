}
int DoAcoustIdWebRequest( vlc_object_t *p_obj, acoustid_fingerprint_t *p_data )
{
    int i_ret;
    int i_status;
    struct webrequest_t request = { NULL, NULL, NULL };
    if ( !p_data->psz_fingerprint ) return VLC_SUCCESS;
    i_ret = asprintf( & request.psz_url,
              "http://fingerprint.videolan.org/acoustid.php?meta=recordings+tracks+usermeta+releases&duration=%d&fingerprint=%s",
              p_data->i_duration, p_data->psz_fingerprint );
    if ( i_ret < 1 ) return VLC_EGENERIC;
    vlc_cleanup_push( cancelDoAcoustIdWebRequest, &request );
    msg_Dbg( p_obj, "Querying AcoustID from %s", request.psz_url );
    int i_saved_flags = p_obj->i_flags;
    p_obj->i_flags |= OBJECT_FLAGS_NOINTERACT;
    request.p_stream = stream_UrlNew( p_obj, request.psz_url );
    p_obj->i_flags = i_saved_flags;
    if ( !request.p_stream )
    {
        i_status = VLC_EGENERIC;
        goto cleanup;
    }
    /* read answer */
    i_ret = 0;
    for( ;; )
    {
        int i_read = 65536;
        if( i_ret >= INT_MAX - i_read )
            break;
        request.p_buffer = realloc_or_free( request.p_buffer, 1 + i_ret + i_read );
        if( !request.p_buffer )
        {
            i_status = VLC_ENOMEM;
            goto cleanup;
        }
        i_read = stream_Read( request.p_stream, &request.p_buffer[i_ret], i_read );
        if( i_read <= 0 )
            break;
        i_ret += i_read;
    }
    stream_Delete( request.p_stream );
    request.p_stream = NULL;
    request.p_buffer[ i_ret ] = 0;
    int i_canc = vlc_savecancel();
    if ( ParseJson( p_obj, request.p_buffer, & p_data->results ) )
    {
        msg_Dbg( p_obj, "results count == %d", p_data->results.count );
    } else {
        msg_Dbg( p_obj, "No results" );
    }
    vlc_restorecancel( i_canc );
    i_status = VLC_SUCCESS;
cleanup:
    vlc_cleanup_run( );
    return i_status;
}
