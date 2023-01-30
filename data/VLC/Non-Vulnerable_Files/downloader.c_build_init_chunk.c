}
static chunk_t *build_init_chunk( stream_t *s )
{
    chunk_t *ret = calloc( 1, sizeof( chunk_t ) );
    if( unlikely( ret == NULL ) )
        goto build_init_chunk_error;
    ret->size = SMOO_SIZE;
    ret->data = malloc( SMOO_SIZE );
    if( !ret->data )
        goto build_init_chunk_error;
    if( build_smoo_box( s, ret->data ) == VLC_SUCCESS)
        return ret;
    free( ret->data );
build_init_chunk_error:
    free( ret );
    msg_Err( s, "build_init_chunk failed" );
    return NULL;
}
