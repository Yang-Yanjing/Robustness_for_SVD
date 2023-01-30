};
static void cancelDoAcoustIdWebRequest( void *p_arg )
{
    struct webrequest_t *p_request = (struct webrequest_t *) p_arg;
    if ( p_request->p_stream )
        stream_Delete( p_request->p_stream );
    free( p_request->psz_url );
    free( p_request->p_buffer );
}
