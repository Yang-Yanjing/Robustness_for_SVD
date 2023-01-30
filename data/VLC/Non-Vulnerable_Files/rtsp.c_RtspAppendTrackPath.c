#define TRACK_PATH_SIZE (sizeof("/trackID=999") - 1)
char *RtspAppendTrackPath( rtsp_stream_id_t *id, const char *base )
{
    const char *sep = strlen( base ) > 0 && base[strlen( base ) - 1] == '/' ?
                      "" : "/";
    char *url;
    if( asprintf( &url, "%s%strackID=%u", base, sep, id->track_id ) == -1 )
        url = NULL;
    return url;
}
